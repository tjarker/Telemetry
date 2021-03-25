/*
  @author Pierre Molinaro
  ACAN library

  Slightly modified example
  
  For CAN instructions for Teensy board, see https://github.com/pierremolinaro/acan
  For DTU Roadrunners previous use of Teensy, see https://bitbucket.org/dtucar/autonomous-ecocar/src/master/Source/
  For more specific examples of how the CAN was implemented in the Ecocar, see
  https://bitbucket.org/dtucar/ecocar/src/master/Code/CanLib/canbus.cpp
  https://bitbucket.org/dtucar/ecocar/src/master/Code/CanLib/canbus.h
  https://bitbucket.org/dtucar/ecocar/src/master/Code/CanLib/canbusKeywords.h
*/

/* TODO:
 * Make function / new file to receive data from nRF24.
 * Find / make Excel file with priority of sensors
*/

#include <Arduino.h>
#include <ACAN.h> //3.6 h-file

// struct for CAN frame
CANMessage msg;

static unsigned gReceivedFrameCount0 = 0 ;
static unsigned gReceivedFrameCount1 = 0 ;
static unsigned gSentFrameCount0 = 0 ;
static unsigned gSentFrameCount1 = 0 ;

static const unsigned MESSAGE_COUNT = 10 * 1000 ;

void setup() {
  while (!Serial) {}
  Serial.begin(9600);

  // Configuration of CAN0 (pin #3 and #4) and CAN1 (pin #33 and #34)
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
  errorCode = ACAN::can1.begin(settings);
  if (0 == errorCode){
    Serial.println("can1 ok");
  } else {
    Serial.print("Error can1: 0x");
    Serial.println(errorCode, HEX);
  }
}

void loop(){
  // Send message from CAN0
  const bool ok = ACAN::can0.tryToSend(msg);
  if (gSentFrameCount0 < MESSAGE_COUNT){
    // Create even identifier
    msg.id = millis() & 0x7FE;
    msg.len = 8;
    for (uint8_t i=0; i<8; i++){
      msg.data[i] = i;
    }
    const bool ok = ACAN::can0.tryToSend(msg);
    if (ok) {
      gSentFrameCount0++;
    }
  }
  // Send message from CAN1
  if (gSentFrameCount1 < MESSAGE_COUNT){
    // Odd identifier
    msg.id = (millis() & 0x7FE) | 1;
    msg.len = 8;
    // Send frame with CAN1
    const bool ok = ACAN::can1.tryToSend(msg);
    if (ok) {
      gSentFrameCount1++;
    }
  }

  // Receive message from CAN0
  if (ACAN::can0.available()){
    ACAN::can0.receive(msg);
    gReceivedFrameCount0++;
    Serial.print("Received: ");
    Serial.println(gReceivedFrameCount1);
  }
  // Receive message from CAN1
  if (ACAN::can1.available()){
    ACAN::can1.receive(msg);
    gReceivedFrameCount1++;
    Serial.print("Received: ");
    Serial.println(gReceivedFrameCount1);
  }
}