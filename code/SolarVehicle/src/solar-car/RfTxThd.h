/**
 * This file contains the function for the thread responsible for streaming CAN
 * messages to the support vehicle.
 */

#ifndef __RF_TX_THD_H__
#define __RF_TX_THD_H__

#include <ChRt.h>

#include "ThreadState.h"
#include "Fifo.h"
#include "RFfunctions.h"
#include "MutexLocker.h"
#include "Measure.h"

#include "solar-car/Mutexes.h"

/**
 * A bundle used for passsing all relevant resources to the radio thread
 */
struct rfTxWorkerBundle{
    Fifo<CanTelemetryMsg> *fifo;
    ThreadState *state;
};

// the working area for the thread is 512 bytes
THD_WORKING_AREA(waRfWorker,1024);

THD_FUNCTION(rfWorker, arg){

  rfTxWorkerBundle *bundle = (rfTxWorkerBundle*) arg;
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
    
    chSysLock();
    Serial.println("Calling rftransmit");
    MEASURE("RF"){RFtransmit((BaseTelemetryMsg*)msg,32);}
    chSysUnlock();
    
    fifo->advance(&fifoReadIndex);
        
  }
  
}

#endif