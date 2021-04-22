#include "RF24Transceiver.h"

// RF24Transceiver() Constructor member function
// Takes the SPI CE and CSN pins, as well as the radioNumber (0 or 1) as input
RF24Transceiver::RF24Transceiver(int CE_PIN, int CSN_PIN, bool radioNumber)
{
    // Assigns member variables according to constructor arguments
    m_CE_PIN = CE_PIN;
    m_CSN_PIN = CSN_PIN; 
    m_radioNumber = radioNumber;  
}

// init() member function
// Takes no arguments.
void RF24Transceiver::init()
{
    // Initializes RF24 class member
    m_radio = new RF24(m_CE_PIN, m_CSN_PIN);                    // Initialize RF24 object
    m_radio->begin();                                           
    m_radio->setPALevel(RF24_PA_LOW);                           // Set Power Amplifier level 
    m_radio->setDataRate(RF24_250KBPS);                         // Set Data Rate
    m_radio->enableDynamicPayloads();           
    m_radio->enableAckPayload();
    m_radio->setRetries(DELAY, COUNT);                          // Sets number of retries and delay between each retry
    m_radio->openWritingPipe(address[!m_radioNumber]);
    m_radio->openReadingPipe(1, address[m_radioNumber]);
    m_radio->startListening();                                  // Starts RX mode
}

// transmit() member function
// Takes char array and its size to send. size cannot be greater than 32 bytes (null-terminated)
void RF24Transceiver::transmit(char &message, int size)
{
    m_radio->stopListening();                                   // Starts TX mode
    bool report = m_radio->write(&message, size);               // Send message and wait for acknowledge
    if (report){    // Checks if message was delivered
        Serial.print(F("Transmission successful! "));           // message was delivered
    } else {
        Serial.println(F("Transmission failed or timed out"));  // message was not delivered
    }
}

// receive() member function
// Takes no arguments, prints received message to serial
void RF24Transceiver::receive()
{
    m_radio->startListening();                                  // Starts RX mode
    char message[32];                                           // Messages cannot be larger than 32 bytes (null-terminated)
    uint8_t pipe; 
    if (m_radio->available(&pipe)){                             // Check if transmitter is sending message
        m_radio->read(&message, sizeof(message));               // Read message
        Serial.println(message);                                // Print message
        m_radio->writeAckPayload(1, &message, sizeof(message)); // Send acknowledge payload
    } 
}