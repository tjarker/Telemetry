// #ifdef TEENSY40_BOARD
// radioNumber = 0;
// #else 
// radioNumber = 1; 
// #endif

#include <Arduino.h>
#include <SPI.h>
#include "RF24.h"

#define COUNT 5     // Number of transmission retries
#define DELAY 15    // Delay between retries (= DELAY * 250 us + 250 us)

RF24 radio(9, 10);
bool radioNumber = 0;
static const byte address[][6] = {"00001", "00002"};    // TX/RX byte addresses

void RFinit(); 
void RFtransmit(); 
void RFreceive(); 