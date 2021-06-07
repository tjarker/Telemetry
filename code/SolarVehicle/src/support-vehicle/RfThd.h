#ifndef __RF_THD_H__
#define __RF_THD_H__

#include <ChRt.h>
#include "ThreadState.h"
#include "RFfunctions.h"
#include "Fifo.h"

// A bundle used for passsing all relevant resources to each thread
struct threadBundle
{
    Fifo<BaseTelemetryMsg> *fifo;
    ThreadState *state;
};

// the working area for the radioReceiverThread is 1024 bytes
THD_WORKING_AREA(waRadioReceiverThread,1024);

THD_FUNCTION(radioReceiverThread, arg){

  threadBundle *radioReceiverBundle = (threadBundle*) arg;
  ThreadState *radioReceiverState = radioReceiverBundle->state;
  Fifo<BaseTelemetryMsg> *radioReceiverFifo = radioReceiverBundle->fifo;
  chThdSleepMilliseconds(100); 
  while(!radioReceiverState->terminate){
    Serial.println("radioReceiverThread");
    //radioReceiverFifo->waitForSpace();

    if(radioReceiverState->pause){
      radioReceiverState->suspend();
    }

    if(RFreceive(radioReceiverFifo->head())){
      radioReceiverFifo->moveTail();
      radioReceiverFifo->signalData();
    }  
  }
  
}

// the working area for the radioTransmitterThread is 1024 bytes
THD_WORKING_AREA(waRadioTransmitterThread, 1024);

THD_FUNCTION(radioTransmitterThread, arg)
{
  threadBundle *radioTransmitterBundle = (threadBundle*) arg;
  ThreadState *radioTransmitterState = radioTransmitterBundle->state;
  Fifo<BaseTelemetryMsg> *radioTransmitterFifo = radioTransmitterBundle->fifo;
  while(!radioTransmitterState->terminate){
    Serial.println("radioTransmitterThread");
    //radioTransmitterFifo->waitForData();

    if(radioTransmitterState->pause){
      radioTransmitterState->suspend();
    }
    
    if (RFtransmit(radioTransmitterFifo->head(), 32)){
      radioTransmitterFifo->moveHead(); 
      radioTransmitterFifo->signalSpace();
    }
    chThdSleepMilliseconds(1000);

  }
  
}

// the working area for the serialReceiverThread is 1024 bytes
THD_WORKING_AREA(waSerialReceiverThread, 1024);

THD_FUNCTION(serialReceiverThread, arg)
{
  threadBundle *serialReceiverBundle = (threadBundle*) arg; 
  ThreadState *serialReceiverState = serialReceiverBundle->state; 
  Fifo<BaseTelemetryMsg> *serialReceiverFifo = serialReceiverBundle->fifo;

  while (!serialReceiverState->terminate) {
    Serial.println("serialReceiverThread");
    //serialReceiverFifo->waitForData();

    if (serialReceiverState->pause) {
      serialReceiverState->suspend();
    }

    if (!serialReceiverFifo->empty()){
      BaseTelemetryMsg *msg = serialReceiverFifo->head();
      char str[128];
      Serial.println(msg->toString(str, sizeof(str)));
      // print to JSON file
      serialReceiverFifo->moveHead();
      serialReceiverFifo->signalSpace(); 
    }
    chThdSleepMilliseconds(1000);

  } 
}

// the working area for the serialTransmitterThread is 1024 bytes
THD_WORKING_AREA(waSerialTransmitterThread, 1024);

THD_FUNCTION(serialTransmitterThread, arg)
{
  threadBundle *serialTransmitterBundle = (threadBundle*) arg;
  ThreadState *serialTransmitterState = serialTransmitterBundle->state; 
  Fifo<BaseTelemetryMsg> *serialTransmitterFifo = serialTransmitterBundle->fifo; 
  
  BaseTelemetryMsg *msg; 

  while (!serialTransmitterState->terminate){
    //serialTransmitterFifo->waitForSpace();
    Serial.println("serialTransmitterThread");
    if (serialTransmitterState->pause) {
      serialTransmitterState->suspend();
    }

    msg->randomize(); 

    if (!serialTransmitterFifo->full()){
      if (Serial.available()){
        msg = serialTransmitterFifo->tail(); 
        //Serial.readBytes((char *)msg, 32);
        serialTransmitterFifo->moveTail();
        serialTransmitterFifo->signalData();
      }
    }
    chThdSleepMilliseconds(1000);
  }
}

#endif