/***************************************************************************
 * @file    blackBoxThd.h                                                  *
 * @author  Tjark Petersen                                                 *
 * @brief   This file contains the function for the thread responsible for *
 *          saving CAN messages from the system FIFO into the blackbox.    *
 ***************************************************************************/

#ifndef __BLACKBOX_THD_H__
#define __BLACKBOX_THD_H__
#include <ChRt.h>
#include "ThreadState.h"
#include "BlackBox.h"
#include "Fifo.h"
#include "MutexLocker.h"
#include "telemetry/solar-car/Mutexes.h"

/**
 * @brief A bundle used for passsing all relevant resources to the radio thread
*/
struct BlackboxWorkerBundle
{
    Fifo<CanTelemetryMsg> *fifo;
    ThreadState *state;
    BlackBox *bb;
};

// The working area for the thread is 1024 bytes
THD_WORKING_AREA(blackBoxWorker, 1024);

/**
 * @brief Thread function for blackBoxThd. 
 * @param arg, typecast to BlackBoxWorkerBundle pointer.
*/ 
THD_FUNCTION(canWorkerFunc, arg){

  BlackboxWorkerBundle *bundle = (BlackboxWorkerBundle*) arg;
  ThreadState *state = bundle->state;
  Fifo<CanTelemetryMsg> *fifo = bundle->fifo;
  BlackBox *bb = bundle->bb;
  CanTelemetryMsg *msg;

  uint32_t lastNum = 0;

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

      if(msg->data64 != (lastNum+1)){
        Serial.println("BBThd:\t\tBlack box missed messages!");
      }
      lastNum = (uint32_t)msg->data64;


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