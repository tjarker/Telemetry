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
#include "encryption.h"

#include "solar-car/Mutexes.h"

/**
 * A bundle used for passsing all relevant resources to the radio thread
 */
struct systemThdBundle{
    ThreadState *canReceiverState;
    ThreadState *blackBoxWorkerState;
    ThreadState *rfWorkerState;
    Security *sec;
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
      uint32_t count = 0;
      for(uint32_t i = 0; i < 32; i++) {
        count += ((uint8_t*)&msg)[i];
      }
      if(!count) {
        RFinit();
        Serial.println("Restarted Radio...........................");
      } else {
        char str[64];
      msg.toString(str,64);
      Serial.print("RF received: ");Serial.println(str);
      }
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

        case 'b':
          if(!blackBoxWorkerState->pause){
            Serial.println("Pausing BlackBox Thread...");
            blackBoxWorkerState->pause = true;
          } else {
            Serial.println("Resuming BlackBox Thread...");
            blackBoxWorkerState->wakeUp();
          }
          break;
        case 'r':
          if(!rfWorkerState->pause){
            Serial.println("Pausing RF Tx Thread...");
            rfWorkerState->pause = true;
          } else {
            Serial.println("Resuming RF Tx Thread...");
            rfWorkerState->wakeUp();
          }
          break;
        case 'c':
          if(!canReceiverState->pause){
            Serial.println("Pausing CAN receiver Thread...");
            canReceiverState->pause = true;
          } else {
            Serial.println("Resuming CAN receiver Thread...");
            canReceiverState->wakeUp();
          }
          break;
        default:
          break;
      }
    }

    chThdYield();
        
  }
  
}

#endif