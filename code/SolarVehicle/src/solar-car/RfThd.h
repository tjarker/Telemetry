/**
 * This file contains the function for the thread responsible for streaming CAN
 * messages to the support vehicle.
 */

#ifndef __RF_THD_H__
#define __RF_THD_H__

#include <ChRt.h>

#include "ThreadState.h"
#include "BlackBox.h"
#include "Fifo.h"
#include "RFfunctions.h"

#include "solar-car/Mutexes.h"

/**
 * A bundle used for passsing all relevant resources to the radio thread
 */
struct rfWorkerBundle{
    Fifo<CanTelemetryMsg> *fifo;
    ThreadState *state;
};

// the working area for the thread is 512 bytes
THD_WORKING_AREA(waRfWorker,1024);

THD_FUNCTION(rfWorker, arg){

  rfWorkerBundle *bundle = (rfWorkerBundle*) arg;
  ThreadState *state = bundle->state;
  Fifo<CanTelemetryMsg> *fifo = bundle->fifo;

  CanTelemetryMsg *msg;

  uint32_t fifoReadIndex = 0;

  WITH_MTX(serialMtx){Serial.println("Starting RF Transmitting thread...");}

  while(!state->terminate){
   
    fifo->waitForData();

    if(state->pause){
      state->suspend();
    }

    msg = fifo->get(fifoReadIndex);

    Serial.println("hello");

    WITH_MTX(serialMtx){
      Serial.print("rf worker msg nr. ");
      Serial.println((uint32_t)msg->data64);
    }
    
    WITH_MTX(rfMTX){
      chSysLock();
      Serial.println("Calling rftransmit");
      MEASURE("RF"){RFtransmit((BaseTelemetryMsg*)msg,32);}
      chSysUnlock();
    }
    
    fifo->advance(&fifoReadIndex);
        
  }
  
}

#endif