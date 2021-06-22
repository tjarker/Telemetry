#ifndef __BLACKBOX_H__
#define __BLACKBOX_H__

#include <ChRt.h>
#include <SdFat.h>
#include <RingBuf.h>
#include <Arduino.h>
#include <TimeLib.h>

#include "Measure.h"
#include "TelemetryMessages.h"


#define SD_CONFIG  SdioConfig(FIFO_SDIO)


class BlackBox: public SdFs
{
    private:

        char fileName[64]; // the current file name
        FsFile file; // the current file

        RingBuf<FsFile, 1024> sdBuffer; // a ring buffer to buffer writes to the SD card
        uint32_t switchTrigger, logsWritten = 0;
        char str[128];
    //----------------------------------------------------------------------------------------------------------------------
    /* Setup functions */
    
    /**
     * @brief   For now the constructor does nothing
     */
    public: BlackBox(uint32_t switchTrigger){
        this->switchTrigger = switchTrigger;
    }

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
    private: static time_t getTeensy3Time(){
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

    public: void getCurrentFileName(char *buf, uint32_t len) {
        memcpy(buf, fileName, len);
    }

    public: uint32_t getBytesInBuffer() {
        return sdBuffer.bytesUsed();
    }

    public: uint32_t cpyFromBuffer(void *buf, uint32_t len) {
        return sdBuffer.memcpyOut(buf,len);
    }

    /**
     * @brief   Creates a new file on the SD card with a current time label as name and a header connected to the @p logType
     * 
     * @note    If a file is already open, the unprocessed contents of the buffer are written to it and it is closed.
     */
    public: void startNewLogFile(){
        logsWritten = 0;
        if(file.isOpen()){
            sdBuffer.sync();
            file.close();
        }

        updateFileName();
        file.open(fileName, O_RDWR | O_CREAT | O_TRUNC);
        
        sdBuffer.begin(&file);
        sdBuffer.println(CanTelemetryMsg::getHeader());
    }

    /**
     * @brief   Writes all unprocessed contents in the buffer to the currently open file and closes it.
     */
    public: void endLogFile(){
        logsWritten = 0;
        sdBuffer.sync();
        file.truncate();
        /*
        file.rewind();
        while(file.available()){
            Serial.print(file.readString());
        }*/
        file.close();
    }

    public: bool addNewLogStr(CanTelemetryMsg *log){
        log->toString(str,sizeof(str));
        sdBuffer.println(str);
        if(sdBuffer.bytesUsed() >= 800){
            sdBuffer.writeOut(sdBuffer.bytesUsed());
        }
        logsWritten++;
        if(logsWritten >= switchTrigger){
            endLogFile();
            startNewLogFile();
            logsWritten = 0;
            return true;
        }
        return false;
    }

    public: void printLogFiles(){
        FsFile dir = this->open("/");
        dir.ls(&Serial,LS_R);
    }
    public: void printLastLog(){
        FsFile fi;
        char buf[32];
        buf[31] = '\0';
        if(!fi.open(fileName, O_READ)){
            Serial.println("Failed to open!");
        }
        /*
        Serial.println("Starting printing file");
        while(fi.available()){
            Serial.print(fi.readString());
        }*/
        fi.rewind();
        while(fi.available()){
            uint8_t len = fi.readBytes(buf,8);
            buf[len] = '\0';
            Serial.print(buf);
        }
        Serial.println("Done...");
    }
};

#endif