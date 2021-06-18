/**
 * This file contains the function for the thread responsible for streaming CAN
 * messages to the support vehicle.
 */

#ifndef __RF_TX_THD_H__
#define __RF_TX_THD_H__

#include <ChRt.h>

#include "ThreadState.h"
#include "MultiReaderFifo.h"
#include "RFfunctions.h"
#include "MutexLocker.h"
#include "Measure.h"
#include "Encryption.h"

#include "telemetry/solar-car/Mutexes.h"


/**
 * A bundle used for passsing all relevant resources to the radio thread
 */
struct rfTxWorkerBundle{
    Fifo<CanTelemetryMsg> *fifo;
    ThreadState *state;
    Security *sec;
};

// the working area for the thread is 512 bytes
THD_WORKING_AREA(waRfWorker,1024);

THD_FUNCTION(rfWorker, arg){

  rfTxWorkerBundle *bundle = (rfTxWorkerBundle*) arg;
  ThreadState *state = bundle->state;
  Fifo<CanTelemetryMsg> *fifo = bundle->fifo;
  Security *sec = bundle->sec;

  CanTelemetryMsg *msg;


  WITH_MTX(serialMtx){Serial.println("RfTxThd:\tStarting");}

  while(!state->terminate){

    //Serial.println("RFTxThd:\tWaiting for data");
    fifo->waitForData();

    if(state->pause){
      state->suspend();
    }

    MEASURE("RfTxThd:\t"){

      msg = fifo->head();
      
      chSysLock();
      uint16_t encrypted[16];
      //char tempString[128];

      /*WITH_MTX(serialMtx){
        Serial.print("RfTxThd:\t");
        msg->toMessage()->toString(tempString,sizeof(tempString));
        Serial.println(tempString);
      }*/
      
      
      

      /*WITH_MTX(serialMtx){

        Serial.print("RfTxThd:\t");
        for(uint32_t i = 0; i < BaseTelemetryMsg::length(); i++) {
          Serial.print(encrypted[i],HEX); Serial.print(" ");
        }
        Serial.println();
      }*/
      WITH_MTX(rfMTX){
        bool success;

        if(sec->activate) {
          sec->encrypt((uint8_t*)msg,encrypted,BaseTelemetryMsg::length());
          success = RFtransmit(encrypted,BaseTelemetryMsg::length()<<1);
        } else {
          success = RFtransmit(encrypted,BaseTelemetryMsg::length());
        }

        if(success){
          fifo->signalSpace();
          fifo->moveHead();
          Serial.println("RfTxThd:\tMessage sent successfully");
        } else {
          Serial.println("RfTxThd:\tMessage not send or incorrect ACK");
        }
        
      }
      /*WITH_MTX(serialMtx){
        BaseTelemetryMsg decryptedMsg;
        sec->decrypt(encrypted,(uint8_t*)&decryptedMsg,BaseTelemetryMsg::length()<<1);

        Serial.print("RfTxThd:\t");
        decryptedMsg.toString(tempString,sizeof(tempString));
        Serial.println(tempString);
      }*/
      chSysUnlock();

    }
        
  }
  
}

#endif