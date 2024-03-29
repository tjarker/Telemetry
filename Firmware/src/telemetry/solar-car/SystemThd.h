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
#include "Encryption.h"

#include "telemetry/solar-car/Mutexes.h"

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
  Security *sec = bundle->sec;

  BaseTelemetryMsg msg;

  Serial.println("SystemThd:\tStarting");

  uint8_t oldBlackBoxState = 0, oldRfTxState = 0;

  chThdSleepMicroseconds(100); // release in order to allow creation of other threads

  while(true){
   
    radio.startListening();

    if(radio.available()){
      if(sec->activate) {
        uint16_t encrypted[16];
        RFreceive(encrypted,BaseTelemetryMsg::length()<<1);
        sec->decrypt(encrypted,(uint8_t*)&msg,BaseTelemetryMsg::length());
      } else {
        RFreceive(&msg,BaseTelemetryMsg::length()<<1);
      }
      
      char str[64];
      switch(msg.cmd){
        case BROADCAST_CAN:
        {
          Serial.print("SystemThd:\tReceived Rf: ");
          CanTelemetryMsg *ptr = (CanTelemetryMsg*)&msg;
          ptr->toString(str,64);
          Serial.println(str);
          CANMessage canMsg;
          ptr->toCanFrame(&canMsg);
          ACAN::can0.tryToSend(canMsg);
        }
          break;
        case START_LOGGING:
        {
          if(blackBoxWorkerState->pause){
            Serial.println("SystemThd:\tResuming BlackBox Thread");
            blackBoxWorkerState->wakeUp();
          } else {
            Serial.println("SystemThd:\tBlackBox Thread already running");
          }
        }
          break;
        case STOP_LOGGING:
        {
          if(!blackBoxWorkerState->pause){
            Serial.println("SystemThd:\tPausing BlackBox Thread");
            blackBoxWorkerState->pause = true;
          } else {
            Serial.println("SystemThd:\tBlackBox Thread already paused");
          }
        }
          break;
        case START_STREAMING:
        {
          if(rfWorkerState->pause){
            Serial.println("SystemThd:\tResuming streaming of CAN data");
            //radio.powerUp(); 
            rfWorkerState->wakeUp();
          } else {
            Serial.println("SystemThd:\tCAN data is already being streamed");
          }
        }
          break;
        case STOP_STREAMING:
        {
          if(!rfWorkerState->pause){
            Serial.println("SystemThd:\tPausing streaming of CAN data");
            //radio.powerDown(); 
            rfWorkerState->pause = true;
          } else {
            Serial.println("SystemThd:\tStreaming of CAN data is already paused");
          }
        }
          break;
        case SLEEP:
        {
          if(!canReceiverState->pause){
            Serial.println("SystemThd:\tPutting system to sleep");
            canReceiverState->pause = true;
            oldBlackBoxState = !blackBoxWorkerState->pause;
            oldRfTxState = !rfWorkerState->pause;
            rfWorkerState->pause = true;
            blackBoxWorkerState->pause = true;
          } else {
            Serial.println("SystemThd:\tSystem is already asleep");
          }
        }
          break;
        case WAKE_UP:
        {
          if(canReceiverState->pause){
            Serial.println("SystemThd:\tWaking up the system");
            canReceiverState->wakeUp();
            if(oldBlackBoxState) blackBoxWorkerState->wakeUp();
            if(oldRfTxState) rfWorkerState->wakeUp();
          } else {
            Serial.println("SystemThd:\tThe system is already awake");
          }
        }
          break;
        case ENABLE_ENCRYPTION:
        {
          if(!sec->activate){
            Serial.println("SystemThd:\tEnabling Encryption");
            WITH_MTX(rfMTX){sec->activate = true;}
          } else {
            Serial.println("SystemThd:\tEncryption already enabled");
          }
        }
          break;
        case DISABLE_ENCRYPTION:
        {
          if(sec->activate){
            Serial.println("SystemThd:\tDisabling Encryption");
            WITH_MTX(rfMTX){sec->activate = false;}
          } else {
            Serial.println("SystemThd:\tEncryption already disabled");
          }
        }
          break;
        default:
        {
          Serial.print("SystemThd:\tReceived Rf: ");
          msg.toString(str,64);
          Serial.println(str);
        }
      }
      
    }

    if(Serial.available()){
      uint8_t read = Serial.read();
      switch (read){
        case 13:
          Serial.println("SystemThd:\tStopping");
          blackBoxWorkerState->terminate = true;
          canReceiverState->terminate = true;
          rfWorkerState->terminate = true;
          while(true){}
          break;

        case 'b':
          if(!blackBoxWorkerState->pause){
            Serial.println("SystemThd:\tPausing BlackBox Thread");
            blackBoxWorkerState->pause = true;
          } else {
            Serial.println("SystemThd:\tResuming BlackBox Thread");
            blackBoxWorkerState->wakeUp();
          }
          break;
        case 'r':
          if(!rfWorkerState->pause){
            Serial.println("SystemThd:\tPausing RF Tx Thread");
            rfWorkerState->pause = true;
          } else {
            Serial.println("SystemThd:\tResuming RF Tx Thread");
            rfWorkerState->wakeUp();
          }
          break;
        case 'c':
          if(!canReceiverState->pause){
            Serial.println("SystemThd:\tPausing CAN receiver Thread");
            canReceiverState->pause = true;
          } else {
            Serial.println("SystemThd:\tResuming CAN receiver Thread");
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