#define TEENSY36

#include "RF24Transceiver.h"
#include <Arduino.h>

RF24Transceiver radio(9, 10, 0); // CE pin, CSN pin, radioNumber (0 or 1)

void setup() 
{
  delay(2000);
  Serial.begin(9600);
  
  while(!Serial){}
  radio.init();
}

void loop() 
{
  
  //char text[] = "Hello mein freund";
  //radio.transmit(*text, sizeof(text)); 
  radio.receive();
}

/*
#include <Arduino.h>
#include <ACAN.h>
#include "BlackBox.h"
#include "RF24Transceiver.h"
#include "StampedCANMessage.h"




//-------------------------------------------------------------------------------------



//BlackBox bb;

// Initialize RF24Transceiver object
RF24Transceiver radio(9, 10, 1);    // CE pin, CSN pin, radioNumber (0 or 1)


void setup(){
  Serial.begin(9600); // Baud rate
  while(!Serial){}
  pinMode(LED_BUILTIN,OUTPUT);

  ACANSettings settings(125 * 1000); // next three lines took 49 ns
  ACAN::can0.begin(settings);

  Serial.println("Starting bouncing messages!");

  //if(!bb.init()){
  //  Serial.println("Failed to initialize black box");
  //}

  //bb.startNewLogFile();

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
    Serial.println("Transmitting");
    //radio.transmit(&msg);    // Transmit CAN frame string
    char text[] = "Hello";
    radio.transmit(*text,sizeof(text));
    Serial.println("Done");
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
*/