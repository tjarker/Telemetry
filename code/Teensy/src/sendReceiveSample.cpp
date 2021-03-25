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
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

// struct for CAN frame
CANMessage msg;

static unsigned gReceivedFrameCount0 = 0 ;
static unsigned gReceivedFrameCount1 = 0 ;
static unsigned gSentFrameCount0 = 0 ;
static unsigned gSentFrameCount1 = 0 ;

static const unsigned MESSAGE_COUNT = 10 * 1000 ;

uint8_t read_data(int i){

	ifstream in("sample.csv");

    string line, field;

    vector< vector<string> > array;  // the 2D array
    vector<string> v;                // array of values for one line only

    while ( getline(in,line) )    // get next line in file
    {
        v.clear();
        stringstream ss(line);

        while (getline(ss,field,','))  // break line into comma delimitted fields
        {
            v.push_back(field);  // add each field to the 1D array
        }

        array.push_back(v);  // add the 1D array to the 2D array
    }

    array[i][3];
}

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
  for (int i = 1; i < 119; i++){
    // Read identifier from array
    msg.id = read_data(i);
    msg.len = 8;
    for (int j=0; j<8; j++){
      msg.data[j] = i;
    }
    const bool ok = ACAN::can0.tryToSend(msg);
    if (ok) {
      gSentFrameCount0++;
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