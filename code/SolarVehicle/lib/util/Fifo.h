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

    public: T *get(uint32_t index){
        return &fifo[index];
    }

    public: void waitForSpace(){
        chSemWait(&spaceSem);
    }

    public: void waitForData(){
        chSemWait(&dataSem);
    }

    public: void advance(uint32_t *index){
        *index = *index < (size - 1) ? *index + 1 : 0; 
    }

    public: void signalRead(){
        chSemSignal(&spaceSem);
    }

    public: void signalWrite(){
        chSemSignal(&dataSem);
    }
    
    public: uint32_t getSize(){
        return size;
    }

    public: void clear(){
        memset(fifo,0,size*sizeof(T));
    }

};

#endif