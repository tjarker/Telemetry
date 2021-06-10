#ifndef __RF_FUNCTIONS__
#define __RF_FUNCTIONS__
#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include "TelemetryMessages.h"

#define IRQ_PIN 2 
#define CE_PIN 9
#define CSN_PIN 10

#define COUNT 5     // Number of transmission retries
#define DELAY 15    // Delay between retries (= DELAY * 250 us + 250 us)

#ifdef TEENSY40_BOARD
bool radioNumber = 0;
#elif defined(ARDUINO_BOARD)
bool radioNumber = 1;
#else
bool radioNumber = 1;
#endif

RF24 radio(CE_PIN, CSN_PIN);                            // CE and CSN pins
static const byte address[][6] = {"00001", "00002"};    // TX/RX byte addresses

// RFinit() function
// Takes no arguments.
void RFinit()
{
    if(!radio.begin()){Serial.println("Radio not working!");}
    radio.setPALevel(RF24_PA_LOW);                              // Set Power Amplifier level
    radio.setDataRate(RF24_1MBPS);                              // Set Data Rate
    radio.enableDynamicPayloads();
    //radio.setAutoAck(true);
    //radio.enableAckPayload();
    radio.setRetries(DELAY, COUNT);                             // Sets number of retries and delay between each retry
    //pinMode(IRQ_PIN, INPUT);
    //attachInterrupt(digitalPinToInterrupt(IRQ_PIN), RFinterrupt, FALLING); 
    //radio.maskIRQ(1, 1, 0);
    radio.openWritingPipe(address[!radioNumber]);
    radio.openReadingPipe(1, address[radioNumber]);
    radio.startListening();                                     // Starts RX mode
}

// First transmit() function
// Takes char array and its size to send. size cannot be greater than 32 bytes (null-terminated)
bool RFtransmit(void *buf, uint8_t len)
{
    radio.stopListening();                                      // Starts TX mode
    bool report = radio.write(buf, len);                       // Send message and wait for acknowledge
    radio.startListening();
    if (report){    // Checks if message was delivered
        //Serial.print(F("Transmission successful! "));           // message was delivered
<<<<<<< HEAD
    /*if (radio.isAckPayloadAvailable()){                     // Checks for ACK packet from RX
=======
        /*if (radio.isAckPayloadAvailable()){                     // Checks for ACK packet from RX
>>>>>>> f0ff33aafd6fa0098c0325dcfea4bdc82aa3a1c7
            uint8_t ack[32], *ori = (uint8_t*)buf;
            radio.read(&ack, len);                              // Loads ACK packet into msg
            for(uint8_t i = 0; i< 32; i++){
                if(ack[i] != ori[i]){}
                    radio.startListening(); 
                    Serial.println("Character");
                    return false;
<<<<<<< HEAD
            }  
            return true;
        }*/
        //Serial.println();
=======
            }
            radio.startListening();
            return true;
        }*/
        //Serial.println();
        radio.startListening();
>>>>>>> f0ff33aafd6fa0098c0325dcfea4bdc82aa3a1c7
        return true;
    } else {
        //Serial.println(F("Transmission failed or timed out"));  // message was not delivered
        radio.startListening();
        return false;
    }
}

// First receive() function
// Takes no arguments, prints received message to serial
bool RFreceive(void *buf, uint8_t len)
{
    radio.startListening();                                     // Starts RX mode
    uint8_t pipe;
    if (radio.available(&pipe)){                                // Check if transmitter is sending message
        radio.read(buf, len);                               // Read message, cannot be larger than 32 bytes (null-terminated)
        //radio.writeAckPayload(1, buf, len);                 // Send acknowledge payload
        return true;
    }
    return false;
}

#endif