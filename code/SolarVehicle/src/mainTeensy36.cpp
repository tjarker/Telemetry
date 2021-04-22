#include <ChRt.h>
#include <Arduino.h>
#include <ACAN.h>
#include "BlackBox.h"
#include "RF24Transceiver.h"
#include "util.h"


//-------------------------------------------------------------------------------------


#define LOG_TYPE BBCANMessage

BlackBox<LOG_TYPE> bb;

// Initialize RF24Transceiver object
RF24Transceiver radio(9, 10, 0);    // CE pin, CSN pin, radioNumber (0 or 1)

void chSetup(){
  chSysInit();
}

void setup(){
  Serial.begin(9600); // Baud rate
  while(!Serial){}
  pinMode(LED_BUILTIN,OUTPUT);

  ACANSettings settings(125 * 1000); // next three lines took 49 ns
  ACAN::can0.begin(settings);

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
    Serial.println();
    radio.transmit(msg.toString());    // Transmit CAN frame string
  }

  if(Serial.available() && Serial.read() == 13){
    Serial.println("\n\nStopping execution...");
    bb.endLogFile();
    bb.printLogFiles();
    bb.printLastLog();
    bb.~BlackBox();
    while(true){}
  }

  chThdSleepMilliseconds(100);

}
