#include <Arduino.h>
#include <ACAN.h>
#include <ChRt.h>
#include "BlackBox.h"
#include "RFfunctions.cpp"
#include "TelemetryMessages.h"


// Size of fifo in records.
const size_t FIFO_SIZE = 32;

// Count of data records in fifo.
SEMAPHORE_DECL(fifoData, 0);

// Count of free buffers in fifo.
SEMAPHORE_DECL(fifoSpace, FIFO_SIZE);

CanTelemetryMsg CanMsgFifo[FIFO_SIZE];

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


THD_WORKING_AREA(waCanReceiver,256);
THD_FUNCTION(canReceiverThd, arg){
  CANMessage frame;
  CanTelemetryMsg *msg;

  uint32_t count = 0;
  size_t fifoHead = 0;

  WITH_MTX(serialMtx){Serial.println("Starting Receiver...");}

  while(globalState.isLogging){
    
    WITH_MTX(serialMtx){Serial.println("Hello");}

    msg = &CanMsgFifo[fifoHead];

    msg->cmd = RECEIVED_CAN;
    msg->data64 = count++;
    msg->id = 0x10A;

    //CanMsgFifo[fifoHead].update(&frame);
    chSemSignal(&fifoData);

    // Advance FIFO index.
    fifoHead = fifoHead < (FIFO_SIZE - 1) ? fifoHead + 1 : 0;
    chThdSleepMilliseconds(1000);
  }
}

BSEMAPHORE_DECL(runCanWorker,1);

THD_WORKING_AREA(waCanWorker,512);
THD_FUNCTION(canWorkerFunc, arg){
  ThreadState *state = (ThreadState*)arg;

  //CanTelemetryMsg *msg;

  size_t fifoTail = 0;

  WITH_MTX(serialMtx){Serial.println("Starting listener...");}

  while(globalState.isLogging){
   
    chSemWait(&fifoData);

    CanTelemetryMsg *msg = &CanMsgFifo[fifoTail];


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

    chSemSignal(&fifoSpace);
    // Advance FIFO index.
    fifoTail = fifoTail < (FIFO_SIZE - 1) ? fifoTail + 1 : 0;
        
  }

  
}

ThreadState canWorkerState;


THD_WORKING_AREA(waTest,256);
THD_FUNCTION(testerFun,arg){
  uint32_t count = 0;
  while(true){
    BaseTelemetryMsg base;
    CanTelemetryMsg *bla = (CanTelemetryMsg*)&base;
    bla->cmd = RECEIVED_CAN;
    bla->data64 = count++;
    bla->id = 0x10A;
    chSysLock();
    MEASURE_EXEC_TIME("Sending RF"){
      RFtransmit(bla->toMessage(), 32);
    }
    chSysUnlock();
    Serial.println(base.toString());
    chThdSleepMilliseconds(1000);
  }
    
}

void chSetup(){
  chSysInit();
  
  chThdCreateStatic(waCanWorker, sizeof(waCanWorker), NORMALPRIO+1, canWorkerFunc, &canWorkerState);
  chThdCreateStatic(waCanReceiver, sizeof(waCanReceiver), NORMALPRIO+2, canReceiverThd, NULL);
  //chThdCreateStatic(waTest,sizeof(waTest),NORMALPRIO + 1,testerFun,NULL);
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

  /*for(uint32_t i = 0; i < FIFO_SIZE; i++){
    CanMsgFifo[i].cmd = RECEIVED_CAN;
  }*/

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