#ifndef __BLACKBOX_THD_H__
#define __BLACKBOX_THD_H__

#include <ChRt.h>
#include "util/ThreadState.h"
#include "solar-car/Mutexes.h"
#include "blackbox/BlackBox.h"
#include "util/Fifo.h"

struct BlackboxWorkerBundle{
    Fifo<CanTelemetryMsg> *fifo;
    ThreadState *state;
    BlackBox *bb;
};

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