/********************************************************************************
 * @file    RFfunctions.h                                                       *
 * @author  Steffan Martin Kunoy                                                *
 * @brief   Declaration of functions to operate and configure nRF24L01+ module. * 
*********************************************************************************/

#ifndef __RF_FUNCTIONS__
#define __RF_FUNCTIONS__
#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include "TelemetryMessages.h"

#ifdef TEENSY36_BOARD
#define IRQ_PIN 2   // SPI interrupt pin
#define CE_PIN 10    // SPI chip-enable (CE) pin
#define CSN_PIN 9  // SPI chip-select (CSN) pin
#else
#define IRQ_PIN 2   // SPI interrupt pin
#define CE_PIN 9    // SPI chip-enable (CE) pin
#define CSN_PIN 10  // SPI chip-select (CSN) pin
#endif

#define COUNT 5     // Number of transmission retries
#define DELAY 15    // Delay between retries (= DELAY * 250 us + 250 us)

#ifdef TEENSY40_BOARD
bool radioNumber = 0;       
#elif defined(ARDUINO_BOARD)
bool radioNumber = 1;
#else
bool radioNumber = 1;
#endif

RF24 radio(CE_PIN, CSN_PIN);                                    // CE and CSN pins
static const byte address[][6] = {"00001", "00002"};            // TX/RX byte addresses
bool ack = false; 

/**********************************************************
 * @brief   Initializes and configures RF24 class object. *
 * @param   None                                          *
***********************************************************/
void RFinit()
{ 
    if(!radio.begin()){Serial.println("Radio not working!");}
    radio.setPALevel(RF24_PA_LOW);                              // Set Power Amplifier level, choose between MIN, LOW, HIGH, MAX (higher PA level improves range)
    radio.setDataRate(RF24_1MBPS);                              // Set Data Rate, choose between RF24_250KBS, RF24_1MBS, RF24_2MBS (higher Data Rates may cause data loss)
    radio.enableDynamicPayloads();                              // Enable variable data payloads
    radio.setAutoAck(true);                                     // RX node sends an automatic ack packet
    if (ack) radio.enableAckPayload();
    radio.setRetries(DELAY, COUNT);                             // Sets number of retries and delay between each retry
    radio.openWritingPipe(address[!radioNumber]);               // Open writing pipe at address 
    radio.openReadingPipe(1, address[radioNumber]);             // Open reading pipe at address
    radio.startListening();                                     // Starts RX mode
}

/********************************************************************************
 * @brief   Tries to transmit message to RX node.                               *
 * @param   buf Pointer to the data to be sent.                                 *
 * @param   len Number of bytes to be sent. Maximum 32 bytes (null-terminated). * 
 * @return  True if message was succesfully transmitted, otherwise false.       *
*********************************************************************************/
bool RFtransmit(void *buf, uint8_t len)
{
    radio.stopListening();                              // Starts TX mode
    bool report = radio.write(buf, len);                // Send message and wait for acknowledge
    radio.startListening();
    if (report){                                        // Checks if message was delivered
        if (ack && radio.isAckPayloadAvailable()){      // Checks for ACK packet from RX                    
            radio.read(&buf, len);                      // Loads ACK packet into buffer
        }
        return true;
    } else {
        return false;
    }
}

/**************************************************************************************
 * @brief   Tries to receive message from TX node.                                    *
 * @param   buf Pointer to a buffer where the data should be written.                 *
 * @param   len Maximum number of bytes to be read into the buffer, maximum 32 bytes. *
 * @return  True if a message was succesfully received, otherwise false.              *
***************************************************************************************/
bool RFreceive(void *buf, uint8_t len)
{
    radio.startListening();                             // Starts RX mode
    uint8_t pipe;                                       
    if (radio.available(&pipe)){                        // Check if transmitter is sending message on pipe
        radio.read(buf, len);                           // Read message, cannot be larger than 32 bytes (null-terminated)
        if (ack) radio.writeAckPayload(1, buf, len);    // Send acknowledge payload
        return true;
    }
    return false;
}

#endif