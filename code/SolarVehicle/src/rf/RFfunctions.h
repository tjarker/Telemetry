#ifndef __RF_FUNCTIONS__
#define __RF_FUNCTIONS__
#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include "communication/TelemetryMessages.h"

#define COUNT 5     // Number of transmission retries
#define DELAY 15    // Delay between retries (= DELAY * 250 us + 250 us)

#ifdef TEENSY40_BOARD
bool radioNumber = 0;
#elif defined(ARDUINO_BOARD)
bool radioNumber = 1;
#else
bool radioNumber = 1;
#endif

RF24 radio(9, 10);                                      // CE and CSN pins
static const byte address[][6] = {"00001", "00002"};    // TX/RX byte addresses

// RFinit() function
// Takes no arguments.
void RFinit()
{
    radio.begin();
    radio.setPALevel(RF24_PA_LOW);                              // Set Power Amplifier level
    radio.setDataRate(RF24_1MBPS);                              // Set Data Rate
    radio.enableDynamicPayloads();
    radio.setAutoAck(true);
    radio.enableAckPayload();
    //radio.setRetries(DELAY, COUNT);                             // Sets number of retries and delay between each retry
    radio.openWritingPipe(address[!radioNumber]);
    radio.openReadingPipe(1, address[radioNumber]);
    radio.startListening();                                     // Starts RX mode
}

// First transmit() function
// Takes char array and its size to send. size cannot be greater than 32 bytes (null-terminated)
bool RFtransmit(BaseTelemetryMsg *msg, uint32_t size)
{
    radio.stopListening();                                      // Starts TX mode
    bool report = radio.write(msg, size);                       // Send message and wait for acknowledge
    if (report){    // Checks if message was delivered
        Serial.print(F("Transmission successful! "));           // message was delivered
        if (radio.isAckPayloadAvailable()){                     // Checks for ACK packet from RX
            radio.read(msg, size);                              // Loads ACK packet into msg
            Serial.print(F("Acknowledge received: "));
            char str[64];
            msg->toString(str,sizeof(str));
            Serial.print(str);                                  // Prints ACK packet
        }
        Serial.println();
        return true;
    } else {
        Serial.println(F("Transmission failed or timed out"));  // message was not delivered
        return false;
    }
}

// First receive() function
// Takes no arguments, prints received message to serial
bool RFreceive(BaseTelemetryMsg *received)
{
    radio.startListening();                                     // Starts RX mode
    uint8_t pipe;
    if (radio.available(&pipe)){                                // Check if transmitter is sending message
        radio.read(received, 32);                               // Read message, cannot be larger than 32 bytes (null-terminated)
        if (received->cmd == RECEIVED_CAN){
            char tmp[64];
            ((CanTelemetryMsg *)received)->toString(tmp, sizeof(tmp));
            Serial.println(tmp);                                // Print message
        }
        radio.writeAckPayload(1, received, 32);                 // Send acknowledge payload
        return true;
    }
    return false;
}


#endif