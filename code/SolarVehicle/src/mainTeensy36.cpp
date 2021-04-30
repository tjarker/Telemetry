#include <Arduino.h>
#include <ACAN.h>
#include <ChRt.h>
#include "BlackBox.h"
#include "RF24Transceiver.h"
#include "TelemetryMessages.h"





class ThreadState {
  public:
    uint8_t isRunning = true;
    uint8_t terminate = false;
    uint8_t pause = false;
};

class GlobalState {
  public:
    uint8_t isLogging = true;
    uint8_t isTransmittingRT = true;
} globalState;

MUTEX_DECL(serialMtx);
MUTEX_DECL(bbMtx);
MUTEX_DECL(rfMTX);

BSEMAPHORE_DECL(runCanWorker,1);


THD_WORKING_AREA(waCanWorker,512);
THD_FUNCTION(canWorkerFunc, arg){
  ThreadState *state = (ThreadState*)arg;

  CANMessage frame;
  CanTelemetryMsg msg;
  msg.cmd = RECEIVED_CAN;

  while(!state->terminate){

    if(!globalState.isLogging){
      chBSemWait(&runCanWorker);
    }

    
    if(ACAN::can0.receive(frame)){

      msg.update(&frame);

      if(Serial){ // print debug info if a serial listener is attached
        WITH_MTX(serialMtx){
          Serial.println(msg.toString(HEX));
        }
      }
      
      if(globalState.isLogging){ // save CAN message to black box
        WITH_MTX(bbMtx){
          //bb.addNewLogStr(&msg);
        }
      }
         
      if(globalState.isTransmittingRT){  // send real time CAN bus data via RF
        WITH_MTX(rfMTX){
          //msg.toMessage()->toBytes()
        }
      }
      
    }

    chThdSleepMicroseconds(100);
  }

  
}

ThreadState canWorkerState;

void chSetup(){
  chSysInit();
  
  chThdCreateStatic(waCanWorker, sizeof(waCanWorker), NORMALPRIO, canWorkerFunc, &canWorkerState);

}

void setup(){
  Serial.begin(9600);
  while(!Serial){} //needs to be removed when headless!!!!!!!!!!!!!!!!!!!!
  pinMode(LED_BUILTIN,OUTPUT);

  ACANSettings settings(125 * 1000);
  ACAN::can0.begin(settings);

  Serial.println("Starting...");

  chBegin(chSetup);

}



void loop(){

  if(Serial.available()){
    uint8_t read = Serial.read();
    switch (read){
      case 13:
        Serial.println("\n\nStopping...");
        canWorkerState.terminate = true;
        //bb.endLogFile();
        //bb.printLogFiles();
        //bb.printLastLog();
        //bb.~BlackBox();

        while(true){}
        break;
      case 'p':
        if(globalState.isLogging){
          Serial.println("Pausing...");
          globalState.isLogging = false;
        } else {
          Serial.println("Resuming...");
          chBSemSignal(&runCanWorker);
          globalState.isLogging = true;
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