#include <ChRt.h>
#include <SdFat.h>
#include <RingBuf.h>
#include <Arduino.h>
#include <TimeLib.h>
#include "util.h"
#include "logType.h"


#define SD_CONFIG  SdioConfig(FIFO_SDIO)

#define NUM_BUFFERS 4
#define BUFFERS_SIZE 4


template<class DT>
class BlackBox: public SdFs
{
    private:

        char fileName[64]; // the current file name
        FsFile file; // the current file

        RingBuf<FsFile, 512> sdBuffer; // a ring buffer to buffer writes to the SD card



        DT logBufs[NUM_BUFFERS][BUFFERS_SIZE]; // for buffering new log data

        msg_t freeBuffersQueue[NUM_BUFFERS]; // queue holding free buffers
        MAILBOX_DECL(freeBuffers, &freeBuffersQueue, NUM_BUFFERS); // mailbox used to interact with free buffers

        msg_t filledBuffersQueue[NUM_BUFFERS]; // queue holding filled buffers ready to be processed
        MAILBOX_DECL(filledBuffers, &filledBuffersQueue, NUM_BUFFERS); // mailbox used to interact with filled buffers

        DT *writeBuf; // pointer to the head of the buffer currently in use to record logs
        uint32_t writeHead = 0; // index of the next used slot in the current buffer


    //----------------------------------------------------------------------------------------------------------------------
    /* Setup functions */
    
    /**
     * @brief   For now the constructor does nothing
     */
    public: BlackBox(){}

    /**
     * @brief   Synchronizes remaining logs in buffer to SD and uninitializes it
     */
    public: ~BlackBox(){
        endLogFile();
        this->end();
    }

    /**
     * @brief   Initializes the black box. The Teensy 3.6 real time clock is setup up and the buffer system is initialized.
     * 
     * @return              returns false if the initialization failed
     */
    public: bool init(){
        // setup RTC
        setSyncProvider(getTeensy3Time);
        delay(100);
        if (timeStatus()!= timeSet) {
            return false;
        }

        // setup write buffer
        writeBuf = logBufs[0];
        writeHead = 0;
        // setup mailbox
        for(int i = 1; i < NUM_BUFFERS; i++){
            (void)chMBPostTimeout(&freeBuffers, (msg_t)&logBufs[i], TIME_INFINITE);
        }

        if(!this->begin(SD_CONFIG)){
            return false;
        }
        return true;
    }

    /**
     * @brief   Helper function used to access the Teensy 3.6 real time clock
     * 
     * @return              accessor to the RTC time
     */
    public: static time_t getTeensy3Time(){
        return Teensy3Clock.get();
    }

    


    //----------------------------------------------------------------------------------------------------------------------
    /* log file functions */

    /**
     * @brief   Sets the file name field to a string containing the current date and time.
     */
    private: void updateFileName(){
        snprintf(fileName,64,"log__%02d_%02d_%02d__%02d_%02d_%04d.csv",hour(),minute(),second(),day(),month(),year());
    }

    /**
     * @brief   Creates a new file on the SD card with a current time label as name and a header connected to the @p logType
     * 
     * @note    If a file is already open, the unprocessed contents of the buffer are written to it and it is closed.
     */
    public: void startNewLogFile(){
        MEASURE_EXEC_TIME("startNewLogFile"){
            if(file.isOpen()){
                proccessBuffer(writeBuf,writeHead);
                file.close();
            }

            updateFileName();
            file.open(fileName, O_RDWR | O_CREAT | O_TRUNC);
            
            sdBuffer.begin(&file);
            sdBuffer.println(DT::getHeader());
        }
    }

    /**
     * @brief   Writes all unprocessed contents in the buffer to the currently open file and closes it.
     */
    public: void endLogFile(){
        proccessBuffer(writeBuf,writeHead);
        file.close();
    }




    //----------------------------------------------------------------------------------------------------------------------
    /* logging functions */

    /**
     * @brief   Copies a new log frame into the internal buffers
     * 
     * @note    If the current buffer is filled, this function posts the full buffer in the "filled" mailbox 
     *          and pulls a new buffer from the "free" mailbox leading to slightly longer execution time.
     * 
     * @param[in] log           pointer to a data frame of base type @p logType
     */
    public: void addNewLog(DT* log){
        ACT_PLUS_MEAS("addNewLog"){

            // copy log contents into buffer
            logType *buf = (logType*) &writeBuf[writeHead++];
            memcpy(buf,log,sizeof(DT));
            buf->secs = second();
            buf->mins = minute();
            buf->hours = hour();

            // if buf is full, get rid of it and fetch new
            if(writeHead >= BUFFERS_SIZE){
                if(chMBPostTimeout(&filledBuffers, (msg_t)writeBuf, TIME_IMMEDIATE) != MSG_OK){
                    Serial.println("Could not post buffer into mailbox!");
                }
                if(chMBFetchTimeout(&freeBuffers,(msg_t*)&writeBuf,TIME_IMMEDIATE) != MSG_OK){
                    Serial.println("Could not fetch new buffer from mailbox!");
                }
                writeHead = 0;
            }
        }
    }
    
    /**
     * @brief   Gives access to a element in the log buffer specified by index
     * 
     * @param[in] idx           index of the element to be accessed
     * 
     * @return                  pointer to the log frame
     */
    public: DT* readLog(uint32_t idx){
        return &writeBuf[(idx <= 0) ? 0 : (idx >= BUFFERS_SIZE) ? BUFFERS_SIZE-1 : idx];
    }
    /**
     * @brief   Gives access to the last inserted log
     * 
     * @return                  pointer to the last inserted log frame
     */
    public: DT* readLastLog(){
        return readLog(writeHead-1);
    }

    


    //----------------------------------------------------------------------------------------------------------------------
    /* buffer processing functions */

    /**
     * @brief   Prints the specified buffer over serial
     * 
     * @param[in] buf           a pointer to the buffer to be printed
     */
    public: void printBuffer(DT *buf){
        ACT_PLUS_MEAS("printBuffer"){
            for(uint16_t i = 0; i < BUFFERS_SIZE; i++){
                Serial.println((&buf[i])->toString());
            }
        }
    }

    public: void printCurrentBuffer(){
        printBuffer(writeBuf);
    }

    /**
     * @brief   Processes a buffer by prewriting the log into the sdBuffer and then syncing the buffer with the actual
     *          file on the SD card
     * 
     * @param[in] buf           a pointer to the buffer to be processed
     */
    private: void proccessBuffer(DT *buf, uint32_t filledUntil){
        MEASURE_EXEC_TIME("write to ringbuffer"){
            if(filledUntil > BUFFERS_SIZE){
                filledUntil = BUFFERS_SIZE;
            }
            for(uint16_t i = 0; i < filledUntil; i++){
                Serial.println((&buf[i])->toString());
                sdBuffer.println((&buf[i])->toString());
                (&buf[i])->reset();
            }
        }
        MEASURE_EXEC_TIME("write to SD"){sdBuffer.sync();}
    }

    /**
     * @brief   This function is used as a background worker which waits for filled buffers and pushes them onto
     *          the sd card
     */
    public: void sdWorker(){
        DT* buf;

        Serial.println("SDWorker started");

        while(true){
            chMBFetchTimeout(&filledBuffers,(msg_t*)&buf, TIME_INFINITE);
            Serial.println("SDWorker Received buffer!");
            //Serial.println(DT::getHeader());
            //printBuffer((logType*)buf);
            proccessBuffer(buf,BUFFERS_SIZE);
            chMBPostTimeout(&freeBuffers,(msg_t)buf, TIME_INFINITE);
        }
    }

};
