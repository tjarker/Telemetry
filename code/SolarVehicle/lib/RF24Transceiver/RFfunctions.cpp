#include "RFfunctions.h"

// init() member function
// Takes no arguments.
void RFinit()
{
    radio.begin();                                              
    radio.setPALevel(RF24_PA_LOW);                              // Set Power Amplifier level 
    radio.setDataRate(RF24_250KBPS);                            // Set Data Rate
    radio.enableDynamicPayloads();           
    radio.enableAckPayload();
    radio.setRetries(DELAY, COUNT);                             // Sets number of retries and delay between each retry
    radio.openWritingPipe(address[!radioNumber]);
    radio.openReadingPipe(1, address[radioNumber]);
    radio.startListening();                                     // Starts RX mode
}

// First transmit() member function
// Takes char array and its size to send. size cannot be greater than 32 bytes (null-terminated)
void RFtransmit()
{
    radio.stopListening();                                      // Starts TX mode
    char message[] = "Hello Teensy40, copy?";
    bool report = radio.write(&message, sizeof(message));       // Send message and wait for acknowledge
    if (report){    // Checks if message was delivered
        Serial.print(F("Transmission successful! "));           // message was delivered
    } else {
        Serial.println(F("Transmission failed or timed out"));  // message was not delivered
    }
}

// First receive() member function
// Takes no arguments, prints received message to serial
void RFreceive()
{
    radio.startListening();                                  // Starts RX mode
    char message[32];                                        // Messages cannot be larger than 32 bytes (null-terminated)
    uint8_t pipe; 
    if (radio.available(&pipe)){                             // Check if transmitter is sending message
        radio.read(&message, sizeof(message));               // Read message
        Serial.println(message);                                // Print message
        radio.writeAckPayload(1, &message, sizeof(message)); // Send acknowledge payload
    } 
}