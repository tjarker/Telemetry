/**
 * This file contains the function for the system thread, which receives commands from the
 * radio module and configures the system accordingly
 */

#ifndef __SYSTEM_THD_H__
#define __SYSTEM_THD_H__

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
struct systemThdBundle{
    ThreadState *canReceiverState;
    ThreadState *blackBoxWorkerState;
    ThreadState *rfWorkerState;
};

// the working area for the thread is 1024 bytes
THD_WORKING_AREA(waSystemThd,1024);

THD_FUNCTION(systemThd, arg){

  systemThdBundle *bundle = (systemThdBundle*) arg;
  ThreadState *canReceiverState = bundle->canReceiverState;
  ThreadState *rfWorkerState = bundle->rfWorkerState;
  ThreadState *blackBoxWorkerState = bundle->blackBoxWorkerState;

  BaseTelemetryMsg msg;

  Serial.println("Starting System Thread...");

  chThdSleepMicroseconds(100); // release in order to allow creation of other threads

  while(true){
   
    if(RFreceive(&msg)){
      char str[64];
      msg.toString(str,64);
      Serial.println(str);
    }

    if(Serial.available()){
      uint8_t read = Serial.read();
      switch (read){
        case 13:
          Serial.println("\n\nStopping...");
          blackBoxWorkerState->terminate = true;
          canReceiverState->terminate = true;
          rfWorkerState->terminate = true;
          while(true){}
          break;

        case 'p':
          if(!canReceiverState->pause){
            Serial.println("Pausing...");
            canReceiverState->pause = true;
          } else {
            Serial.println("Resuming...");
            canReceiverState->wakeUp();
          }
          break;
        default:
          break;
      }
    }
        
  }
  
}

#endif