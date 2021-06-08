/**
 * This file contains the function for the thread responsible for receiving new CAN messages and putting them into the system FIFO
 */

#ifndef __CAN_RECEVER_THD_H__
#define __CAN_RECEVER_THD_H__

#include <ChRt.h>

#include "Fifo.h"
#include "ThreadState.h"
#include "MutexLocker.h"
#include "TelemetryMessages.h"

#include "solar-car/Mutexes.h"

/**
 * A bundle used for passsing all relevant resources to the thread
 */
struct CanReceiverBundle{
    Fifo<CanTelemetryMsg> *fifo;
    ThreadState *state;
};

// the working area for the thread is 256 bytes
THD_WORKING_AREA(waCanReceiver,1024);

THD_FUNCTION(canReceiverThd, arg){

  CanReceiverBundle *bundle = (CanReceiverBundle*) arg;
  ThreadState *state = bundle->state;
  Fifo<CanTelemetryMsg> *fifo = bundle->fifo;

  CANMessage frame;
  CanTelemetryMsg *msg;

  uint32_t fifoWriteIndex = 0;

  WITH_MTX(serialMtx){Serial.println("Starting CAN receiver thread...");}

  while(!state->terminate){

    if(state->pause){
      state->suspend();
    }


    if(ACAN::can0.receive(frame)){
      chSysLock();
      msg = fifo->get(fifoWriteIndex);
      msg->update(&frame);
      fifo->signalData();
      fifo->advance(&fifoWriteIndex);
      Serial.print("Received CAN [");Serial.print(fifoWriteIndex);Serial.println("]");
      chSysUnlock();
    }

    chThdYield();

  }
}

#endif