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
#include "encryption.h"

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

  WITH_MTX(serialMtx){Serial.println("Starting RF Transmitting thread...");}

  while(!state->terminate){
   
    Serial.println("RX Tx waiting for data");
    fifo->waitForData();
    Serial.println("RF Tx got data");

    if(state->pause){
      state->suspend();
    }

    msg = fifo->head(readerId);
    
    chSysLock();
    Serial.println("Calling rftransmit");
    char bytes[32];
    memcpy(bytes,msg,32);
    Serial.print("Pre-Encrypted Message is: ");
    for(uint32_t i = 0; i < 32; i++) {
      Serial.print((uint8_t)bytes[i]);Serial.print(" ");
    }
    Serial.println();
    sec->encrypt(bytes,32);
    Serial.print("Encrypted Message is: ");
    for(uint32_t i = 0; i < 32; i++) {
      Serial.print((uint8_t)bytes[i]);Serial.print(" ");
    }
    Serial.println();
    sec->decrypt(bytes,32);
    Serial.print("Decrypted Message is: ");
    for(uint32_t i = 0; i < 32; i++) {
      Serial.print((uint8_t)bytes[i]);Serial.print(" ");
    }
    Serial.println();
    RFtransmit((BaseTelemetryMsg*)bytes,32);
    chSysUnlock();
    
    fifo->moveHead(readerId);
        
  }
  
}

#endif