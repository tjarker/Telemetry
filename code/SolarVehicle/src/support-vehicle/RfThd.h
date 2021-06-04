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

// the working area for the thread is 512 bytes
THD_WORKING_AREA(waRXthread,512);

THD_FUNCTION(RXthread, arg){

  rfWorkerBundle *bundle = (rfWorkerBundle*) arg;
  ThreadState *state = bundle->state;
  Fifo<BaseTelemetryMsg> *fifo = bundle->fifo;

  //CanTelemetryMsg *msg;

  uint32_t fifoTail = 0;

  //WITH_MTX(serialMtx){Serial.println("Starting RF Transmitting thread...");}

  while(!state->terminate){
   
    fifo->waitForData();

    if(state->pause){
      state->suspend();
    }

    //msg = fifo->fifoHead();

    Serial.println("hello");
    
    WITH_MTX(rfMTX){
      chSysLock();
      RFreceive(fifo->fifoHead());
      chSysUnlock();
    }
    
    fifo->fifoMoveHead();
        
  }
  
}
// the working area for the thread is 512 bytes
THD_WORKING_AREA(waTXthread,512);

THD_FUNCTION(TXthread, arg){

  rfWorkerBundle *bundle = (rfWorkerBundle*) arg;
  ThreadState *state = bundle->state;
  Fifo<BaseTelemetryMsg> *fifo = bundle->fifo;

  BaseTelemetryMsg *msg;

  uint32_t fifoHead = 0;

  while(!state->terminate){
   
    fifo->waitForData();

    if(state->pause){
      state->suspend();
    }

    msg = fifo->get(fifoHead);

    Serial.println("hello");
    
    WITH_MTX(rfMTX){
      chSysLock();
      RFtransmit(msg,32);
      chSysUnlock();
    }
    
    fifo->advance(&fifoHead);
        
  }
  
}
#endif