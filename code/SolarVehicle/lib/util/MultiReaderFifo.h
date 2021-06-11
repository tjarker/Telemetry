#ifndef __MULTI_READER_FIFO_H__
#define __MULTI_READER_FIFO_H__

#include <ChRt.h>



/**
 * A Fifo with built in semaphores
 */
template<typename T>
class MultiReaderFifo {
    private: 
        semaphore_t spaceSem, *dataSems;
        T *fifo;
        uint32_t writeIndex = 0, *readIndices;  
        uint32_t size, readers;

    public: MultiReaderFifo(uint32_t size, uint8_t readers){
        this->size  = size;
        this->readers = readers;
        spaceSem    = _SEMAPHORE_DATA(spaceSem,(cnt_t)size);
        fifo        = new T[size];
        dataSems    = new semaphore_t[readers];
        readIndices = new uint32_t[readers];

        for(uint32_t i = 0; i < readers; i++) {
            dataSems[i] = _SEMAPHORE_DATA(dataSems[i],0);
            readIndices[i] = 0;
        }
    }

    public: ~MultiReaderFifo(){
        delete [] fifo;
        delete [] readIndices;
        delete [] dataSems;
    }

    public: T *head(uint32_t readerId) {
        return &fifo[readIndices[readerId]];
    }

    public: T *head() {
        return &fifo[readIndices[0]];
    }

    public: T *tail() {
        return &fifo[writeIndex];
    }

    // puts the caller thread to sleep until free fifo slots are present
    public: void waitForSpace(){
        chSemWait(&spaceSem);
    }

    // puts the caller thread to sleep until new data is present
    public: void waitForData(uint32_t readerId){
        chSemWait(&(dataSems[readerId]));
    }

    public: void waitForData() {
        chSemWait(&(dataSems[0]));
    }

    public: void moveTail() {
        writeIndex = advance(writeIndex); // TODO: push read indices when they are overtaken
        for(uint32_t i = 0; i < readers; i++) {
            if(writeIndex == readIndices[i])
                readIndices[i] = advance(readIndices[i]);
        }
    }

    public: void moveHead(uint32_t readerId) {
        readIndices[readerId] = advance(readIndices[readerId]);
    }

    public: void moveHead() {
        readIndices[0] = advance(readIndices[0]);
    }

    // returns a pointer index to the next fifo slot
    public: uint32_t advance(uint32_t index){
        return index < (size - 1) ? index + 1 : 0;  
    }

    // signals that an element has been read and that the slot is free for new data
    public: void signalSpace(){ // TODO: when is there space with two readers? favor one or the one that lackes behind?
        chSemSignal(&spaceSem);
    }

    // signals that new data is present
    public: void signalData(){
        for(uint32_t i = 0; i < readers; i++) {
            chSemSignal(&(dataSems[i]));
        }
    }
    
    // returns the size of the fifo
    public: uint32_t getSize(){
        return size;
    }

    // set all fifo entries to 0
    public: void clear(){
        memset(fifo,0,size*sizeof(T));
    }
    
    // Prints the contents of the fifo buffer for debugging
    public: void printContents(){
        for (int i = 0; i < size; i++){
            Serial.print(i + " : " + fifo[i]);
        }
        Serial.println();
    }
    

    // Used to check whether fifo is full
    public: bool full(){
        return chSemGetCounterI(&spaceSem) == 0;
    }

    // Used to check whether fifo is empty
    public: bool empty(){
        return chSemGetCounterI(&spaceSem) == size;
    }
};

#endif