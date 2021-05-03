#include <Arduino.h>
#include <SPI.h>
#include "RF24.h"
#include "TelemetryMessages.h"

#define COUNT 5     // Number of transmission retries
#define DELAY 15    // Delay between retries (= DELAY * 250 us + 250 us)

#ifdef TEENSY40_BOARD
bool radioNumber = 0;
#elif defined(ARDUINO_BOARD)
bool radioNumber = 1;
#else
bool radioNumber = 1;
#endif

RF24 radio(9, 10);  // CE and CSN pins
static const byte address[][6] = {"00001", "00002"};    // TX/RX byte addresses

// RFinit() function
// Takes no arguments.
void RFinit()
{
    radio.begin();                                              
    radio.setPALevel(RF24_PA_LOW);                              // Set Power Amplifier level 
    radio.setDataRate(RF24_1MBPS);                              // Set Data Rate
    radio.enableDynamicPayloads();           
    radio.enableAckPayload();
    radio.setRetries(DELAY, COUNT);                             // Sets number of retries and delay between each retry
    radio.openWritingPipe(address[!radioNumber]);
    radio.openReadingPipe(1, address[radioNumber]);
    radio.startListening();                                     // Starts RX mode
}

// First transmit() function
// Takes char array and its size to send. size cannot be greater than 32 bytes (null-terminated)
void RFtransmit(BaseTelemetryMsg *msg, int size)
{
    radio.stopListening();                                      // Starts TX mode
    bool report = radio.write(msg, size);                       // Send message and wait for acknowledge
    if (report){    // Checks if message was delivered
        Serial.print(F("Transmission successful! "));           // message was delivered
    } else {
        Serial.println(F("Transmission failed or timed out"));  // message was not delivered
    }
}

// First receive() function
// Takes no arguments, prints received message to serial
void RFreceive(BaseTelemetryMsg *received)
{
    radio.startListening();                                     // Starts RX mode
    uint8_t pipe; 
    if (radio.available(&pipe)){                                // Check if transmitter is sending message
        radio.read(received, 32);                               // Read message, cannot be larger than 32 bytes (null-terminated)
        Serial.println(received->toString());                   // Print message
        radio.writeAckPayload(1, received, 32);                 // Send acknowledge payload
    }
}