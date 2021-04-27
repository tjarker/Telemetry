#include <Arduino.h>
#include <ACAN.h>
#include "BlackBox.h"
#include "RF24Transceiver.h"
#include "StampedCANMessage.h"

//-------------------------------------------------------------------------------------

void setup(){
  Serial.begin(9600); // Baud rate
  while(!Serial){}
  pinMode(LED_BUILTIN,OUTPUT);

  ACANSettings settings(125 * 1000); // next three lines took 49 ns
  ACAN::can0.begin(settings);

  Serial.println("Starting bouncing messages!");
  chBegin(chSetup);
}

void loop(){

  CANMessage frame;
  StampedCANMessage msg;

  if (ACAN::can0.receive(frame)){
    Serial.println("Received!");
    msg.update(&frame);
    Serial.println(msg.toString());
    //bb.addNewLogStr(&msg);
  }

  if(Serial.available() && Serial.read() == 13){
    Serial.println("\n\nStopping execution...");
    //bb.endLogFile();
    //bb.printLogFiles();
    //bb.printLastLog();
    //bb.~BlackBox();
    while(true){}
  }


}