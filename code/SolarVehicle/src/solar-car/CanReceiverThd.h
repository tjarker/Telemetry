/**
 * This file contains the function for the thread responsible for receiving new CAN messages and putting them into the system FIFO
 */

#ifndef __CAN_RECEVER_THD_H__
#define __CAN_RECEVER_THD_H__

#include <ChRt.h>

#include "MultiReaderFifo.h"
#include "ThreadState.h"
#include "MutexLocker.h"
#include "TelemetryMessages.h"

#include "solar-car/Mutexes.h"

/**
 * A bundle used for passsing all relevant resources to the thread
 */
struct CanReceiverBundle{
    MultiReaderFifo<CanTelemetryMsg> *fifo;
    ThreadState *state;
};

// the working area for the thread is 256 bytes
THD_WORKING_AREA(waCanReceiver,1024);

THD_FUNCTION(canReceiverThd, arg){

  CanReceiverBundle *bundle = (CanReceiverBundle*) arg;
  ThreadState *state = bundle->state;
  MultiReaderFifo<CanTelemetryMsg> *fifo = bundle->fifo;

  CANMessage frame;
  CanTelemetryMsg *msg;

  WITH_MTX(serialMtx){Serial.println("Starting CAN receiver thread...");}

  while(!state->terminate){

    if(state->pause){
      state->suspend();
    }

    if(ACAN::can0.receive(frame)){
      chSysLock();
      Serial.println("Can Receiver Thread 3");
      msg = fifo->tail();
      Serial.println("Can Receiver Thread 4");
      msg->update(&frame);
      Serial.println("Can Receiver Thread 5");
      fifo->signalData();
      Serial.println("Can Receiver Thread 6");
      fifo->moveTail();
      Serial.println("Can Receiver Thread 7");
      chSysUnlock();
    }

    chThdYield();

  }
}

#endif