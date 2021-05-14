#include <Arduino.h>
#include <ACAN.h>
#include <ChRt.h>
#include "BlackBox.h"
#include "RFfunctions.cpp"
#include "TelemetryMessages.h"
#include "Fifo.h"


Fifo<CanTelemetryMsg> canFifo(32);

class ThreadState {
  public:
    uint8_t isRunning = true;
    uint8_t terminate = false;
    uint8_t pause = false;
    thread_reference_t trp = NULL;

  public: void suspend(){
    chSysLock();
    chThdSuspendS(&trp);
    chSysUnlock();
  }
  public: void wakeUp(msg_t msg){
    chSysLock();
    chThdResumeI(&trp,msg);
    pause = false;
    chSysUnlock();
  }
};

class GlobalState {
  public:
    uint8_t isLogging = true;
    uint8_t isTransmittingRT = true;
} globalState;

MUTEX_DECL(serialMtx);
MUTEX_DECL(bbMtx);
MUTEX_DECL(rfMTX);


THD_WORKING_AREA(waCanReceiver,256);
THD_FUNCTION(canReceiverThd, arg){

  ThreadState *state = (ThreadState*) arg;

  CANMessage frame;
  CanTelemetryMsg *msg;

  uint32_t count = 0;
  uint32_t fifoHead = 0;

  WITH_MTX(serialMtx){Serial.println("Starting Receiver...");}

  while(!state->terminate){

    if(state->pause){
      state->suspend();
    }

    msg = canFifo.get(fifoHead);

    msg->cmd = RECEIVED_CAN;
    msg->data64 = count++;
    msg->id = 0x10A;

    canFifo.signalWrite();
    canFifo.advance(&fifoHead);

    chThdSleepMilliseconds(1000);
  }
}



THD_WORKING_AREA(waCanWorker,512);
THD_FUNCTION(canWorkerFunc, arg){
  ThreadState *state = (ThreadState*)arg;

  CanTelemetryMsg *msg;

  uint32_t fifoTail = 0;

  WITH_MTX(serialMtx){Serial.println("Starting listener...");}

  while(!state->terminate){
   
    canFifo.waitForData();

    if(state->pause){
      state->suspend();
    }

    msg = canFifo.get(fifoTail);


    if(Serial){
      WITH_MTX(serialMtx){
        char str[64];
        msg->toString(str,64);
        Serial.println(str);
      }
    }
    
    chSysLock();
    RFtransmit((BaseTelemetryMsg*)msg,32);
    chSysUnlock();

    canFifo.signalRead();
    canFifo.advance(&fifoTail);
        
  }

  
}

ThreadState canWorkerState;
ThreadState canReceiverState;

void chSetup(){
  chSysInit();
  
  chThdCreateStatic(waCanWorker, sizeof(waCanWorker), NORMALPRIO+1, canWorkerFunc, &canWorkerState);
  chThdCreateStatic(waCanReceiver, sizeof(waCanReceiver), NORMALPRIO+2, canReceiverThd, &canReceiverState);
}

CANMessage frame;
CanTelemetryMsg msg;

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
        if(!canWorkerState.pause){
          Serial.println("Pausing...");
          canWorkerState.pause = true;
        } else {
          Serial.println("Resuming...");
          canWorkerState.wakeUp(NULL);
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