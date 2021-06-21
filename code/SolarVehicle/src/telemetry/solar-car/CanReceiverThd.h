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

#include "telemetry/solar-car/Mutexes.h"

/**
 * A bundle used for passsing all relevant resources to the thread
 */
struct CanReceiverBundle{
    Fifo<CanTelemetryMsg> *bbFifo;
    Fifo<CanTelemetryMsg> *rfFifo;
    ThreadState *state;
};

// the working area for the thread is 256 bytes
THD_WORKING_AREA(waCanReceiver,1024);

THD_FUNCTION(canReceiverThd, arg){

  CanReceiverBundle *bundle = (CanReceiverBundle*) arg;
  ThreadState *state = bundle->state;
  Fifo<CanTelemetryMsg> *bbFifo = bundle->bbFifo;
  Fifo<CanTelemetryMsg> *rfFifo = bundle->rfFifo;

  CANMessage frame;

  WITH_MTX(serialMtx){Serial.println("CanReceiverThd:\tStarting");}

  while(!state->terminate){

    if(state->pause){
      state->suspend();
    }
    
    if(ACAN::can0.available()){
      MEASURE("CanReceiverThd:\t"){

        ACAN::can0.receive(frame);


        bbFifo->tail()->update(&frame);
        bbFifo->moveTail();
        bbFifo->signalData();

        rfFifo->tail()->update(&frame);
        rfFifo->moveTail();
        rfFifo->signalData();

      }
    }

    

    chThdYield();

  }
}

#endif