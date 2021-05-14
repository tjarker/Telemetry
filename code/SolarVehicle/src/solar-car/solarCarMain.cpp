#include <Arduino.h>
#include <ACAN.h>
#include <ChRt.h>
#include "blackbox/BlackBox.h"
#include "rf/RFfunctions.h"
#include "communication/TelemetryMessages.h"
#include "util/Fifo.h"
#include "util/ThreadState.h"
#include "solar-car/blackBoxThd.h"
#include "solar-car/CanReceiverThd.h"
#include "solar-car/Mutexes.h"
#include "solar-car/RfThd.h"


class GlobalState {
  public:
    uint8_t isLogging = true;
    uint8_t isTransmittingRT = true;
} globalState;

Fifo<CanTelemetryMsg> canFifo(32);
BlackBox bb;
ThreadState blackBoxWorkerState;
ThreadState canReceiverState;
ThreadState rfWorkerState;

void chSetup();
void setup(){
  Serial.begin(9600);
  while(!Serial){} //needs to be removed when headless!!!!!!!!!!!!!!!!!!!!
  pinMode(LED_BUILTIN,OUTPUT);

  //ACANSettings settings(125 * 1000);
  //if(ACAN::can0.begin(settings)){Serial.println("CAN setup failed!");}
  RFinit();

  Serial.println("Starting...");  

  canFifo.clear();

  chBegin(chSetup);

}

void chSetup(){
  chSysInit();
  
  BlackboxWorkerBundle blackBoxWorkerBundle = {.fifo = &canFifo, .state = &blackBoxWorkerState, .bb = &bb};
  chThdCreateStatic(blackBoxWorker, sizeof(blackBoxWorker), NORMALPRIO+1, canWorkerFunc, &blackBoxWorkerBundle);
  rfWorkerBundle rfWorkerBundle = {.fifo = &canFifo, .state = &rfWorkerState};
  chThdCreateStatic(waRfWorker,sizeof(waRfWorker),NORMALPRIO + 1, rfWorker, &rfWorkerBundle);
  CanReceiverBundle canReceiverBundle = {.fifo = &canFifo, .state = &canReceiverState};
  chThdCreateStatic(waCanReceiver, sizeof(waCanReceiver), NORMALPRIO+2, canReceiverThd, &canReceiverBundle);
}

void loop(){
  

  if(Serial.available()){
    uint8_t read = Serial.read();
    switch (read){
      case 13:
        Serial.println("\n\nStopping...");
        blackBoxWorkerState.terminate = true;
        canReceiverState.terminate = true;
        rfWorkerState.terminate = true;
        //bb.endLogFile();
        //bb.printLogFiles();
        //bb.printLastLog();
        //bb.~BlackBox();

        while(true){}
        break;
      case 'p':
        if(!blackBoxWorkerState.pause){
          Serial.println("Pausing...");
          canReceiverState.pause = true;
        } else {
          Serial.println("Resuming...");
          canReceiverState.wakeUp();
        }
        break;
      default:
        break;
    }
    
  }

/*
  if(radio.available()){
    BaseTelemetryMsg msg;
    radio.receive(&msg);
    switch(msg.cmd){
      case BROADCAST_CAN:
        CanTelemetryMsg *can = (CanTelemetryMsg*)&msg;
        WITH_MTX(serialMtx){Serial.println(can->toString());}
        break;
      // ....
    }
  }
*/

}


/*
msg.cmd = STOP_LOGGING;
  msg.id = 0x74F;
  msg.rtr = true;
  msg.data64 = 0xAABBCCDDEEFFLLU;
  msg.len = 8;
  msg.s = 2;
  msg.m = 3;
  msg.h = 4;

  uint8_t *msgBytes = msg.toMessage()->toBytes();
  for(uint8_t i = 0; i < 32; i++){
    Serial.print(msgBytes[i],HEX);
    Serial.print(" ");
  }
  Serial.println();


  BaseTelemetryMsg *base = (BaseTelemetryMsg*)msgBytes;
  Serial.println(base->cmd,HEX);
  for(uint8_t i = 0; i < 31; i++){
    Serial.print(base->data[i],HEX);
    Serial.print(" ");
  }
  Serial.println();

  FileStreamMsg fsm;
  fsm.cmd = FILE_STREAM_DATA;
  snprintf(fsm.str, 31, "Hello World!");

  Serial.println(fsm.toString());

  base = fsm.toMessage();
  Serial.println(base->toString());
*/