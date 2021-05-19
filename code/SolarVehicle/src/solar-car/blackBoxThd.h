/**
 * This file contains the function for the thread responsible for saving CAN messages from the system FIFO into the blackbox
 */

#ifndef __BLACKBOX_THD_H__
#define __BLACKBOX_THD_H__

#include <ChRt.h>

#include "ThreadState.h"
#include "BlackBox.h"
#include "Fifo.h"
#include "MutexLocker.h"

#include "solar-car/Mutexes.h"

/**
 * A bundle used for passsing all relevant resources to the radio thread
 */
struct BlackboxWorkerBundle{
    Fifo<CanTelemetryMsg> *fifo;
    ThreadState *state;
    BlackBox *bb;
};

// the working area for the thread is 512 bytes
THD_WORKING_AREA(blackBoxWorker,512);

THD_FUNCTION(canWorkerFunc, arg){

  BlackboxWorkerBundle *bundle = (BlackboxWorkerBundle*) arg;
  ThreadState *state = bundle->state;
  Fifo<CanTelemetryMsg> *fifo = bundle->fifo;
  BlackBox *bb = bundle->bb;

  CanTelemetryMsg *msg;

  uint32_t fifoTail = 0;

  WITH_MTX(serialMtx){Serial.println("Starting listener...");}

  while(!state->terminate){
   
    fifo->waitForData();

    if(state->pause){
      state->suspend();
    }

    msg = fifo->get(fifoTail);


    if(Serial){
      WITH_MTX(serialMtx){
        char str[64];
        msg->toString(str,64);
        Serial.println(str);
      }
    }

    fifo->signalRead();
    fifo->advance(&fifoTail);
        
  }

  
}

#endif