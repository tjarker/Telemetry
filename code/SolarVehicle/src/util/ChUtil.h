#ifndef __CH_UTIL__
#define __CH_UTIL__
#include "ChRt.h"
#include "communication/TelemetryMessages.h"

// fifoBuffer class declaration
class fifoBuffer 
{
    // private member variables
  private: 
    static const size_t _fifoSize = 32;     // size of array
    BaseTelemetryMsg _fifoArray[_fifoSize]; // FIFO queue data structure 
    size_t _fifoHead;                       // index of head
    size_t _fifoTail;                       // index of tail
    // public member functions
  public:
    fifoBuffer();                           // class constructor    
    BaseTelemetryMsg * fifoHead();          // pointer to fifoHead
    BaseTelemetryMsg * fifoTail();          // pointer to fifoTail
    void fifoMoveHead();
    void fifoMoveTail(); 
    semaphore_t fifoData; 
    semaphore_t fifoSpace;
};

// fifoBuffer constructor member function
// Takes no arguments.
fifoBuffer::fifoBuffer()
{
  // fifoHead and fifoTail initialized to zero  
  // Remember: always read from head, write to tail
  _fifoHead = 0;                             
  _fifoTail = 0;
  // declare fifoData and fifoSpace semaphores 
  SEMAPHORE_DECL(fifoData, 0);
  SEMAPHORE_DECL(fifoSpace, _fifoSize);
}

// fifoHead() member function
// returns a pointer to BaseTelemetryMsg object at head of fifoArray
BaseTelemetryMsg * fifoBuffer::fifoHead()
{
    return (BaseTelemetryMsg*)(_fifoArray + _fifoHead);
}

// fifoTail() member function
// returns a pointer to BaseTelemetryMsg object at tail of fifoArray
BaseTelemetryMsg * fifoBuffer::fifoTail()
{
    return (BaseTelemetryMsg*)(_fifoArray + _fifoTail);
}

// fifoMoveHead() member function
// Increments head index
void fifoBuffer::fifoMoveHead()
{
    _fifoHead = _fifoHead < (_fifoSize - 1) ? _fifoHead + 1 : 0;
}

// fifoMoveHead() member function
// Increments tail index
void fifoBuffer::fifoMoveTail()
{
    _fifoTail = _fifoTail < (_fifoSize - 1) ? _fifoTail + 1 : 0; 
}

#endif