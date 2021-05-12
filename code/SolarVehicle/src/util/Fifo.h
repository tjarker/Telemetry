#ifndef __FIFO_H__
#define __FIFO_H__

#include<ChRt.h>

template<typename T>
class Fifo {
    private: 
        semaphore_t spaceSem, dataSem;
        T *fifo;
        uint32_t size;

    public: Fifo(uint32_t size){
        this->size  = size;
        spaceSem    = _SEMAPHORE_DATA(spaceSem,(cnt_t)size);
        dataSem     = _SEMAPHORE_DATA(dataSem,0);
        fifo        = new T[size];
    }

    public: ~Fifo(){
        delete [] fifo;
    }

    public: T *getHead(){
        return &fifo[head];
    }

    public: void advanceHead(){
        head = head < (size - 1) ? head + 1 : 0;
        chSemSignal(&dataSem);
    }

    public: T *getTail(){
        return &fifo[tail];
    }

    public: void advanceTail(){
        tail = tail < (size - 1) ? tail + 1 : 0;
        chSemSignal(&spaceSem);
    }

    public: void waitForSpace(){
        chSemWait(&spaceSem);
    }

    public: void waitForData(){
        chSemWait(&dataSem);
    }

    public: uint32_t advance(uint32_t index){
        return index < (size - 1) ? index + 1 : 0; 
    }

};

#endif