
#include <ChRt.h>
#include <Arduino.h>
#include "BlackBox.h"
#include "ErrorHandler.h"

#define LOG_TYPE CanMessage

BlackBox<LOG_TYPE> bb;

THD_WORKING_AREA(waSdWorker,256);
THD_FUNCTION(sdWorkerFunc, arg){
  ((BlackBox<LOG_TYPE>*) arg)->sdWorker();
}

void chSetup(){
  chSysInit();
  chThdCreateStatic(waSdWorker, sizeof(waSdWorker), NORMALPRIO-1, sdWorkerFunc, &bb);
}

void setup() {

  pinMode(LED_BUILTIN,OUTPUT);

  Serial.begin(9600);

  while(!Serial){}


  if(!bb.init()){
    Serial.println("Failed to initialize black box");
  }

  bb.startNewLogFile();


  chBegin(chSetup);
  
}

void loop() {
  uint32_t counter = 0;

  while(true){
    while(!Serial.available()){
      chThdSleepMilliseconds(100);
    }
    char rd = Serial.read();
    if(rd == 13){
      break;
    }
    CanMessage msg;
    msg.id = counter;
    msg.ext = true;
    msg.rtr = true;
    msg.len = 4;
    msg.data = 1LL*counter*10;
    bb.addNewLog(&msg);
    bb.printCurrentBuffer();
    chThdSleepMilliseconds(100);
    Serial.flush();
    counter++;
  }

  Serial.println("Exiting Program!");

  bb.~BlackBox();

  while(true){}
}