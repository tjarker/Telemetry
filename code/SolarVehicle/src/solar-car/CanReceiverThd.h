#ifndef __CAN_RECEVER_THD_H__
#define __CAN_RECEVER_THD_H__

#include <ChRt.h>
#include "util/Fifo.h"
#include "util/ThreadState.h"
#include "solar-car/Mutexes.h"
#include "util/util.h"
#include "communication/TelemetryMessages.h"


struct CanReceiverBundle{
    Fifo<CanTelemetryMsg> *fifo;
    ThreadState *state;
};


THD_WORKING_AREA(waCanReceiver,256);
THD_FUNCTION(canReceiverThd, arg){

  CanReceiverBundle *bundle = (CanReceiverBundle*) arg;
  ThreadState *state = bundle->state;
  Fifo<CanTelemetryMsg> *fifo = bundle->fifo;

  CANMessage frame;
  CanTelemetryMsg *msg;

  uint32_t count = 0;
  uint32_t fifoHead = 0;

  WITH_MTX(serialMtx){Serial.println("Starting CAN receiver thread...");}

  while(!state->terminate){

    if(state->pause){
      state->suspend();
    }

    msg = fifo->get(fifoHead);

    msg->cmd = RECEIVED_CAN;
    msg->data64 = count++;
    msg->id = 0x10A;

    fifo->signalWrite();
    fifo->advance(&fifoHead);

    chThdSleepMilliseconds(500);
  }
}

#endif