/*
  @author Tjark Petersen, s186083@student.dtu.dk
  @author Steffan Martin Kunoy, s194006@student.dtu.dk
  @author Victor Alexander Hansen, s194027@student.dtu.dk
  
  For CAN instructions for Teensy board, see https://github.com/pierremolinaro/acan
  For DTU Roadrunners previous use of Teensy, see https://bitbucket.org/dtucar/autonomous-ecocar/src/master/Source/
  For more specific examples of how the CAN was implemented in the Ecocar, see
  https://bitbucket.org/dtucar/ecocar/src/master/Code/CanLib/canbus.cpp
  https://bitbucket.org/dtucar/ecocar/src/master/Code/CanLib/canbus.h
  https://bitbucket.org/dtucar/ecocar/src/master/Code/CanLib/canbusKeywords.h
*/

/* TODO:
 * Make function / new file to receive data from nRF24.
 * Configure CreateData function to desired frame. Currently configured for simple
 * example from CANBUS.pdf
 * Some elements are taken from the ACAN library example, would be nice to know
 * what the ACANPrimaryFilter class does, since it is not described in great detail
*/

#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <ACAN.h> //3.6 h-file

// struct for CAN frame
CANMessage msg;

static unsigned gReceivedFrameCount1 = 0;
static unsigned gSentFrameCount0 = 0;
static const unsigned MESSAGE_COUNT = 10 * 1000;

// Frame of CAN message
void CreateData(int t, int error){
  msg.id = 0x0F1; // CAN identifier
  msg.ext = true; // Extended identifier, necessary?
  msg.rtr = true; // Remote transmission request packet type
  msg.len = 8; // Length of data
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
  // Configuration of CAN0 (pin #3 and #4) and CAN1 (pin #33 and #34)
  ACANPrimaryFilter filter [1] = {
    ACANPrimaryFilter(kRemote, kStandard, 0x0F1) // Receive remote standard frames
  };
  ACANSettings settings(125 * 1000); // 125 KBit/s bitrate
  settings.mTxPinIsOpenCollector = true;
  settings.mRxPinHasInternalPullUp = true; // Pull Up enabled

  //Configuration of error codes from CAN0 and CAN1, checks for errors
  uint32_t errorCode = ACAN::can0.begin(settings);
  if (0 == errorCode){
    Serial.println("can0 ok");
  } else {
    Serial.print("Error can0: 0x");
    Serial.println(errorCode, HEX);
  }
  errorCode = ACAN::can1.begin(settings, filter, 1);
  if (0 == errorCode){
    Serial.println("can1 ok");
  } else {
    Serial.print("Error can1: 0x");
    Serial.println(errorCode, HEX);
  }
}

void loop(){
  CreateData(1, 1);
  // Send message from CAN0
  const bool ok = ACAN::can0.tryToSend(msg);
  if (ok){
    gSentFrameCount0 += 1;
    Serial.print("Sent: ");
      Serial.println (gSentFrameCount0);
  }
  // Receive message from CAN1
  if (ACAN::can1.receive(msg)){
    gReceivedFrameCount1 += 1;
    Serial.print("Received: ");
    Serial.println(gReceivedFrameCount1);
  }
}