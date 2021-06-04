#ifndef __FIFO_H__
#define __FIFO_H__

#include <ChRt.h>

/**
 * A Fifo with built in semaphores
 */
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

    // returns a pointer to the element at the given position in the fifo
    public: T *get(uint32_t index){
        return &fifo[index];
    }

    // puts the caller thread to sleep until free fifo slots are present
    public: void waitForSpace(){
        chSemWait(&spaceSem);
    }

    // puts the caller thread to sleep until new data is present
    public: void waitForData(){
        chSemWait(&dataSem);
    }

    // returns a pointer index to the next fifo slot
    public: uint32_t advance(uint32_t index){
        return index < (size - 1) ? index + 1 : 0;  
    }

    // advances the index pointer to the next fifo slot
    public: void advance(uint32_t *index){
        *index = *index < (size - 1) ? *index + 1 : 0;
    }

    // signals that an element has been read and that the slot is free for new data
    public: void signalRead(){
        chSemSignal(&spaceSem);
    }

    // signals that new data is present
    public: void signalWrite(){
        chSemSignal(&dataSem);
    }
    
    // returns the size of the fifo
    public: uint32_t getSize(){
        return size;
    }

    // set all fifo entries to 0
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
        _fifoHead = advance(_fifoHead); 
    }

    // Increments fifoTail pointer
    public: void fifoMoveTail(){
        _fifoTail = advance(_fifoTail);
    }

    // Used to check whether fifo is full
    public: bool fifoFull(){
        return advance(_fifoTail) == _fifoHead;
    }

    public: bool fifoEmpty(){
        return _fifoTail == _fifoHead; 
    }
};

#endif