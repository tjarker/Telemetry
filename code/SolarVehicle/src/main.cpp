#include <ChRt.h>
#include <Arduino.h>
#include <ACAN.h>
#include "CANChatterGen.h"
#include "BlackBox.h"
#include "util.h"

//-------------------------------------------------------------------------------------
// Can bus data generator (simulating can bus chatter for now)
THD_WORKING_AREA(waCanSender,256);
THD_FUNCTION(canSenderFunc, arg){

  Serial.println("Starting sender thread...");

  CANChatterGen msgGen;

  uint32_t lastSend = micros();
  
  while(true){
    if(ACAN::can1.tryToSend(msgGen.newChatter())){
      uint32_t now = micros();
      Serial.printf("%d us between messages\n\n",now-lastSend);
      lastSend = now;
    } else {
      msgGen.reverse();
    }
    chThdSleepMilliseconds(800);
  }
}
//-------------------------------------------------------------------------------------


#define LOG_TYPE BBCANMessage

BlackBox<LOG_TYPE> bb;


void chSetup(){
  chSysInit();
  chThdCreateStatic(waCanSender, sizeof(waCanSender), NORMALPRIO + 1, canSenderFunc, NULL);
}

void setup(){
  Serial.begin(9600); // Baud rate
  while(!Serial){}
  pinMode(LED_BUILTIN,OUTPUT);

  ACANSettings settings(125 * 1000); // next three lines took 49 ns
  ACAN::can0.begin(settings);
  ACAN::can1.begin(settings);


  Serial.println("Starting bouncing messages!");

  if(!bb.init()){
    Serial.println("Failed to initialize black box");
  }

  bb.startNewLogFile();

  chBegin(chSetup);
}

void loop(){

  CANMessage frame;
  BBCANMessage msg;

  

  if (ACAN::can0.receive(frame)){
    Serial.printf("New Message from %d: %" PRIx64 "\n", frame.id, frame.data64);
    msg.msg = frame;
    bb.addNewLogStr(&msg);
  }

  if(Serial.available() && Serial.read() == 13){
    Serial.println("\n\nStopping execution...");
    bb.~BlackBox();
    while(true){}
  }

  chThdSleepMilliseconds(100);

}
