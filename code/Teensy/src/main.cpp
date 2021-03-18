/*
  @author Tjark Petersen, s186083@student.dtu.dk
  @author Steffan Martin Kunoy, s194006@student.dtu.dk
  @author Victor Alexander Hansen, s194027@student.dtu.dk
  
  For CAN instructions for Teensy board, see https://github.com/pierremolinaro/acan
  For DTU Roadrunners previous use of Teensy, see https://bitbucket.org/dtucar/autonomous-ecocar/src/master/Source/
*/
#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <ACAN.h> //3.6 h-file

// struct for CAN frame
CANMessage msg;

void CreateData(int t, int error){
  msg.id = 0x0F1;
  msg.len = 8;
  msg.data[0] = error;
  msg.data[1] = t;
  msg.data[2] = 0x00;
  msg.data[3] = 0x00;
  msg.data[4] = 0x00;
  msg.data[5] = 0x00;
  msg.data[6] = 0x00;
  msg.data[7] = 0x00;
}

void setup() {
  while (!Serial);
  Serial.begin(9600);
  ACANSettings settings (125 * 1000); // 125 KBit/s

}

void loop() {
  // put your main code here, to run repeatedly:
  
  

}