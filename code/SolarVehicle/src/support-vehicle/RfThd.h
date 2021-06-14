#ifndef __RF_THD_H__
#define __RF_THD_H__        

#include <ChRt.h>
#include "ThreadState.h"
#include "RFfunctions.h"
#include "Fifo.h"

// Two FIFO buffers for radio & serial I/O
Fifo<BaseTelemetryMsg> RFoutbox(32), RFinbox(32);
 
// Thread states for all 4 active threads
ThreadState radioReceiverState, radioTransmitterState, serialReceiverState, serialTransmitterState; 
ThreadState radioWorkerState, serialWorkerState; 
// A bundle used for passsing all relevant resources to each thread
struct threadBundle
{
    Fifo<BaseTelemetryMsg> *fifo[2];
    ThreadState *state;
};
/*
// 1024 byte working stack for radioReceiverThread
THD_WORKING_AREA(waRadioReceiverThread, 1024);

// Thread function for radioReceiverThread 
THD_FUNCTION(radioReceiverThread, arg){

  threadBundle *radioReceiverBundle = (threadBundle*) arg;
  ThreadState *radioReceiverState = radioReceiverBundle->state;
  Fifo<BaseTelemetryMsg> *radioReceiverFifo = radioReceiverBundle->fifo;
  Serial.println("radioReceiver");
  //chThdSleepMilliseconds(100);
  while(!radioReceiverState->terminate){
    //Serial.println("radioReceiverThread");

    if (radioReceiverFifo->full()){
      radioReceiverFifo->waitForSpace();
    }

    if (radioReceiverState->pause){
      radioReceiverState->suspend();
    }

    if (RFreceive(radioReceiverFifo->tail())){
      Serial.println("radioReceiverThread");
      //char str[64];
      //Serial.println((radioReceiverFifo->tail())->toString(str,sizeof(str)));
      radioReceiverFifo->moveTail();
      radioReceiverFifo->signalData();
    }
    chThdYield();  
  }
}

// 1024 byte working stack for radioTransmitterThread
THD_WORKING_AREA(waRadioTransmitterThread, 1024);

// Thread function for radioReceiverThread 
THD_FUNCTION(radioTransmitterThread, arg)
{
  threadBundle *radioTransmitterBundle = (threadBundle*) arg;
  ThreadState *radioTransmitterState = radioTransmitterBundle->state;
  Fifo<BaseTelemetryMsg> *radioTransmitterFifo = radioTransmitterBundle->fifo;
  Serial.println("radioTransmitter");
  
  while(!radioTransmitterState->terminate){
    Serial.println("radioTransmitterThread");

    radioTransmitterFifo->waitForData();

    if(radioTransmitterState->pause){
      radioTransmitterState->suspend();
    }
    
    if (RFtransmit(radioTransmitterFifo->head(), 32)){
      radioTransmitterFifo->moveHead(); 
      radioTransmitterFifo->signalSpace();
    } 
  }
}

// 1024 byte working stack for serialReceiverThread
THD_WORKING_AREA(waSerialReceiverThread, 1024);

// Thread function for radioReceiverThread 
THD_FUNCTION(serialReceiverThread, arg)
{
  Serial.println("serialReceiver");
  threadBundle *serialReceiverBundle = (threadBundle*) arg;
  ThreadState *serialReceiverState = serialReceiverBundle->state; 
  Fifo<BaseTelemetryMsg> *serialReceiverFifo = serialReceiverBundle->fifo; 
  BaseTelemetryMsg msg;

  while (!serialReceiverState->terminate){
    Serial.println("serialReceiverThread");
    if (serialReceiverFifo->full()){
      serialReceiverFifo->waitForSpace();
    }
    
    if (serialReceiverState->pause) {
      serialReceiverState->suspend();
    }

    if (Serial){
      memcpy((void*) serialReceiverFifo->tail(), (void*) &msg, sizeof(msg));
      //char str[8]; 
      //Serial.println((serialTransmitterFifo->tail())->toString(str, sizeof(str)));
      serialReceiverFifo->moveTail();
      serialReceiverFifo->signalData();
    }
    
    //chThdSleepMilliseconds(1000);
    chThdYield();
  }
}

// 1024 byte working stack for serialTransmitterThread
THD_WORKING_AREA(waSerialTransmitterThread, 1024);

// Thread function for radioReceiverThread 
THD_FUNCTION(serialTransmitterThread, arg)
{
  threadBundle *serialTransmitterBundle = (threadBundle*) arg; 
  ThreadState *serialTransmitterState = serialTransmitterBundle->state; 
  Fifo<BaseTelemetryMsg> *serialTransmitterFifo = serialTransmitterBundle->fifo;
  Serial.println("serialTransmitter");
  
  while (!serialTransmitterState->terminate) {
    Serial.println("serialTransmitterThread");

    serialTransmitterFifo->waitForData();

    if (serialTransmitterState->pause) {
      serialTransmitterState->suspend();
    }
    
    if (Serial){
      BaseTelemetryMsg *msg = serialTransmitterFifo->head();
      char str[64];
      Serial.println(msg->toString(str, sizeof(str)));
      // print to JSON file
      serialTransmitterFifo->moveHead();
      serialTransmitterFifo->signalSpace(); 
    }
  }
}
*/

THD_WORKING_AREA(waRadioWorkerThread, 2048);

THD_FUNCTION(radioWorkerThread, arg)
{
  threadBundle *radioWorkerBundle = (threadBundle*) arg; 
  ThreadState *radioWorkerState = radioWorkerBundle->state;
  chThdSleepMicroseconds(100); 
  //Fifo<BaseTelemetryMsg> *radioReceiverFifo = radioWorkerBundle->fifo[0], *radioTransmitterFifo = radioWorkerBundle->fifo[1]; 
  BaseTelemetryMsg received; 

  while (!radioWorkerState->terminate){
    
    if (radioWorkerState->pause){
      radioWorkerState->suspend(); 
    }
    if (radio.available()){
      if (RFreceive(&received, 32)){
      Serial.print("Received: ");
      cmd_t r = received.cmd; 
      switch (r){
        case RECEIVED_CAN:
          char str[256]; 
          CanTelemetryMsg *ptr = (CanTelemetryMsg*)&received; 
          ptr->toJSON(str, sizeof(str)); 
          Serial.print(str); 
          break;
        default: 
          break; 
      }
      Serial.println();  
      } else {
        Serial.println("Could not receive message.");
      }
    }
    //if (Serial.available()) chThdSleepMilliseconds(100); 
    chThdYield();
  }
}

THD_WORKING_AREA(waSerialWorkerThread, 2048);

THD_FUNCTION(serialWorkerThread, arg)
{
  threadBundle *serialWorkerBundle = (threadBundle*) arg; 
  ThreadState *serialWorkerState = serialWorkerBundle->state; 
  //Fifo<BaseTelemetryMsg> *serialTransmitterFifo = serialWorkerBundle->fifo[0], *serialReceiverFifo = serialWorkerBundle->fifo[1];
  BaseTelemetryMsg message; 

  while (!serialWorkerState->terminate){
    if (serialWorkerState->pause){
      serialWorkerState->suspend(); 
    } 
     
    if (Serial.available()){
      Serial.readBytes((char*)&message, BaseTelemetryMsg::length());
      if (RFtransmit(&message, BaseTelemetryMsg::length())){
        char str[64]; 
        message.toString(str, sizeof(str));
        Serial.print("Transmitted: "); 
        Serial.println(str); 
        Serial.println("Acknowledge received.");
      } else {
        Serial.println("Transmission failed or timed out.");
      }
    }  
    chThdYield();  
  }
} 

#endif