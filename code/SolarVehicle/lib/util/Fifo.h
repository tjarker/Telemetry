#ifndef __FIFO_H__
#define __FIFO_H__

#include<ChRt.h>

template<typename T>
class Fifo {
    private: 
        semaphore_t spaceSem, dataSem;
        T *fifo;
        size_t _fifoHead, _fifoTail;  
        uint32_t size;

    public: Fifo(uint32_t size){
        this->size  = size;
        spaceSem    = _SEMAPHORE_DATA(spaceSem,(cnt_t)size);
        dataSem     = _SEMAPHORE_DATA(dataSem,0);
        fifo        = new T[size];
        _fifoHead   = 0;
        _fifoTail   = 0;
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

    public: uint32_t advance(uint32_t *index){
        return *index < (size - 1) ? *index + 1 : 0;  
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

    // Returns a pointer to element at fifoEnd index
    public: T *fifoHead(){
        return &fifo[_fifoHead];    
    }

    // Returns a pointer to element at fifoTail index
    public: T *fifoTail(){
        return &fifo[_fifoTail];
    }

    // Increments fifoHead pointer
    public: void fifoMoveHead(){
        advance(&_fifoHead); 
    }

    // Increments fifoTail pointer
    public: void fifoMoveTail(){
        advance(&_fifoTail);
    }

    // Used to check whether fifo is full
    public: bool fifoFull(){
        return advance(&_fifoTail) == _fifoHead;
    }

    public: bool fifoEmpty(){
        return _fifoTail == _fifoHead; 
    }
};

#endif