/**
 * This file contains the function for the thread responsible for streaming CAN
 * messages to the support vehicle.
 */

#ifndef __RF_THD_H__
#define __RF_THD_H__

#include <ChRt.h>
#include "ThreadState.h"
#include "Fifo.h"
#include "RFfunctions.h"
#include "support-vehicle/Mutexes.h"
#include "MutexLocker.h"
/**
 * A bundle used for passsing all relevant resources to the radio thread
 */
struct rfWorkerBundle{
    Fifo<BaseTelemetryMsg> *fifo;
    ThreadState *state;
};

// the working area for the thread is 1024 bytes
THD_WORKING_AREA(waRXthread,1024);

THD_FUNCTION(RXthread, arg){

  rfWorkerBundle *bundle = (rfWorkerBundle*) arg;
  ThreadState *state = bundle->state;
  Fifo<BaseTelemetryMsg> *fifo = bundle->fifo;

  while(!state->terminate){
    fifo->waitForSpace();

    if(state->pause){
      state->suspend();
    }

    if(RFreceive(fifo->fifoHead())){
      fifo->fifoMoveTail();
      fifo->signalData();
    }  

  }
  
}
// the working area for the thread is 1024 bytes
THD_WORKING_AREA(waTXthread, 1024);

THD_FUNCTION(TXthread, arg){

  rfWorkerBundle *bundle = (rfWorkerBundle*) arg;
  ThreadState *state = bundle->state;
  Fifo<BaseTelemetryMsg> *fifo = bundle->fifo;

  CanTelemetryMsg msg; 

  while(!state->terminate){
    fifo->waitForData();

    if(state->pause){
      state->suspend();
    }

    msg.randomize(); 
    
    if (RFtransmit(msg.toMessage(), 32)){
      fifo->fifoMoveHead();
      fifo->signalSpace();
    }
  }
  
}

THD_WORKING_AREA(waSerialThread, 1024);

THD_FUNCTION(serialThread, arg)
{
  rfWorkerBundle *bundle1 = (rfWorkerBundle*) (*(rfWorkerBundle**)arg), *bundle2 = (rfWorkerBundle*) (*(rfWorkerBundle**)arg + 1);
  ThreadState *state1 = bundle1->state, *state2 = bundle2->state; 
  Fifo<BaseTelemetryMsg> *fifo1 = bundle1->fifo, *fifo2 = bundle2->fifo; 

  while (!state1->terminate && !state2->terminate){
    /* */
  }
}
#endif