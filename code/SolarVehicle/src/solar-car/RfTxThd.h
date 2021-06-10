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

#include "solar-car/Mutexes.h"


/**
 * A bundle used for passsing all relevant resources to the radio thread
 */
struct rfTxWorkerBundle{
    MultiReaderFifo<CanTelemetryMsg> *fifo;
    ThreadState *state;
    Security *sec;
};

// the working area for the thread is 512 bytes
THD_WORKING_AREA(waRfWorker,1024);

THD_FUNCTION(rfWorker, arg){

  rfTxWorkerBundle *bundle = (rfTxWorkerBundle*) arg;
  ThreadState *state = bundle->state;
  MultiReaderFifo<CanTelemetryMsg> *fifo = bundle->fifo;
  Security *sec = bundle->sec;

  CanTelemetryMsg *msg;


  uint8_t readerId = 1;

  WITH_MTX(serialMtx){Serial.println("RfTxThd:\tStarting");}

  while(!state->terminate){
   
    //Serial.println("RFTxThd:\tWaiting for data");
    fifo->waitForData(readerId);

    if(state->pause){
      state->suspend();
    }

    MEASURE("RfTxThd:\t"){

      msg = fifo->head(readerId);
      
      chSysLock();
      BaseTelemetryMsg decryptedMsg;
      memcpy(&decryptedMsg,msg,32);
      char tempString[64];
      WITH_MTX(serialMtx){
        Serial.print("RfTxThd:\t");
        decryptedMsg.toString(tempString,64);
        Serial.println(tempString);

        /*sec->encrypt((uint8_t*)&decryptedMsg,32);

        Serial.print("RfTxThd:\t");
        decryptedMsg.toString(tempString,64);
        Serial.println(tempString);

        sec->decrypt((uint8_t*)&decryptedMsg,32);

        Serial.print("RfTxThd:\t");
        decryptedMsg.toString(tempString,64);
        Serial.println(tempString);*/
      }
      WITH_MTX(rfMTX){
        if(!RFtransmit((BaseTelemetryMsg*)&decryptedMsg,32)){
          Serial.println("RfTxThd:\tMessage not send or incorrect ACK");
        } else {
          Serial.println("RfTxThd:\tMessage sent successfully");
        }
      }
      chSysUnlock();
      
      fifo->moveHead(readerId);

    }
        
  }
  
}

#endif