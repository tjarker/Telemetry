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
        size_t _size, _head, _tail;  

    public: Fifo(uint32_t size){
        this->_size  = size;
        this->_head   = 0;
        this->_tail   = 0;
        spaceSem    = _SEMAPHORE_DATA(spaceSem,(cnt_t)_size);
        dataSem     = _SEMAPHORE_DATA(dataSem,0);
        fifo        = new T[_size];
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
        return index < (_size - 1) ? index + 1 : 0;  
    }

    // advances the index pointer to the next fifo slot
    public: void advance(uint32_t *index){
        *index = *index < (_size - 1) ? *index + 1 : 0;
    }

    // signals that an element has been read and that the slot is free for new data
    public: void signalSpace(){
        chSemSignal(&spaceSem);
    }

    // signals that new data is present
    public: void signalData(){
        chSemSignal(&dataSem);
    }
    
    // returns the size of the fifo
    public: uint32_t getSize(){
        return _size;
    }

    // set all fifo entries to 0
    public: void clear(){
        memset(fifo, 0, _size * sizeof(T));
    }
    
    // Prints the contents of the fifo buffer for debugging
    public: void printContents(){
        for (int i = 0; i < _size; i++){
            char str[64]; 
            snprintf(str, sizeof(str), "%-2d : %d", i, fifo[i]); 
            Serial.println(str);
        }
    }

    // Returns a pointer to element at head index
    public: T *head(){
        return &fifo[_head];    
    }

    // Returns a pointer to element at tail index
    public: T *tail(){
        return &fifo[_tail];
    }

    // Increments fifoHead pointer
    public: void moveHead(){
        _head = advance(_head); 
    }

    // Increments tail pointer
    public: void moveTail(){
        _tail = advance(_tail);
    }

    // Used to check whether fifo is full
    public: bool full(){
        return advance(_tail) == _head;
    }

    // Used to check whether fifo is empty
    public: bool empty(){
        return _tail == _head; 
    }
};

#endif