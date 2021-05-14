#ifndef __RF_THD_H__
#define __RF_THD_H__

#include <ChRt.h>
#include "util/ThreadState.h"
#include "solar-car/Mutexes.h"
#include "blackbox/BlackBox.h"
#include "util/Fifo.h"
#include "rf/RFfunctions.h"

struct rfWorkerBundle{
    Fifo<CanTelemetryMsg> *fifo;
    ThreadState *state;
};

THD_WORKING_AREA(waRfWorker,512);
THD_FUNCTION(rfWorker, arg){

  rfWorkerBundle *bundle = (rfWorkerBundle*) arg;
  ThreadState *state = bundle->state;
  Fifo<CanTelemetryMsg> *fifo = bundle->fifo;

  CanTelemetryMsg *msg;

  uint32_t fifoTail = 0;

  WITH_MTX(serialMtx){Serial.println("Starting RF Transmitting thread...");}

  while(!state->terminate){
   
    fifo->waitForData();

    if(state->pause){
      state->suspend();
    }

    msg = fifo->get(fifoTail);

    WITH_MTX(serialMtx){
      Serial.print("rf worker msg nr. ");
      Serial.println((uint32_t)msg->data64);
    }
    
    WITH_MTX(rfMTX){
      chSysLock();
      RFtransmit((BaseTelemetryMsg*)msg,32);
      chSysUnlock();
    }
    
    fifo->advance(&fifoTail);
        
  }

  
}

#endif