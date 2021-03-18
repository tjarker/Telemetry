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

// struct can_frame canMsg;

CANMessage data;

void setup() {
  while (!Serial);
  Serial.begin(9600);
  ACANSettings settings (125 * 1000); // 125 KBit/s

}

void loop() {
  // put your main code here, to run repeatedly:
}
/*problem: initialization with '{...}' expected for aggregate object
// Possibly due to C++ syntax?

void CreateData(int t, int error){
  CANMessage id = 0x0F1;
CANMessage len = 8;
CANMessage data[0] = error;
CANMessage data[1] = t;
CANMessage data[2] = 0x00;
CANMessage data[3] = 0x00;
CANMessage data[4] = 0x00;
CANMessage data[5] = 0x00;
CANMessage data[6] = 0x00;
CANMessage data{7} = 0x00;
}*/