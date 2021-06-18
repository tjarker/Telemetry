/**
 * This file contains the function for the thread responsible for saving CAN messages from the system FIFO into the blackbox
 */

#ifndef __BLACKBOX_THD_H__
#define __BLACKBOX_THD_H__

#include <ChRt.h>

#include "ThreadState.h"
#include "BlackBox.h"
#include "MultiReaderFifo.h"
#include "MutexLocker.h"

#include "telemetry/solar-car/Mutexes.h"

/**
 * A bundle used for passsing all relevant resources to the radio thread
 */
struct BlackboxWorkerBundle{
    Fifo<CanTelemetryMsg> *fifo;
    ThreadState *state;
    BlackBox *bb;
};

// the working area for the thread is 512 bytes
THD_WORKING_AREA(blackBoxWorker,1024);

THD_FUNCTION(canWorkerFunc, arg){

  BlackboxWorkerBundle *bundle = (BlackboxWorkerBundle*) arg;
  ThreadState *state = bundle->state;
  Fifo<CanTelemetryMsg> *fifo = bundle->fifo;
  BlackBox *bb = bundle->bb;

  CanTelemetryMsg *msg;

  WITH_MTX(serialMtx){Serial.println("BBThd:\t\tStarting");}

  while(!state->terminate){

    //Serial.println("BBThd:\t\tWaiting for data");
    fifo->waitForData();

    if(state->pause){
      bb->endLogFile();
      state->suspend();
      bb->startNewLogFile();
    }

    MEASURE("BBThd:\t\t"){

      msg = fifo->head();


      if(Serial){
        WITH_MTX(serialMtx){
          char str[64];
          msg->toString(str,64);
          Serial.print("BBThd:\t\t");
          Serial.println(str);
        }
      }

      if(bb->addNewLogStr(msg)){
        Serial.println("BBThd:\t\tStarting new log file");
      }

      fifo->signalSpace();
      fifo->moveHead();
    }
        
  }

  
}

#endif