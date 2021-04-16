#include "RF24Transceiver.h"

// RF24Transceiver() Constructor member function
RF24Transceiver::RF24Transceiver(int CE_PIN, int CSN_PIN, bool radioNumber)
{
    // Assigns member variables according to constructor arguments
    m_CE_PIN = CE_PIN;
    m_CSN_PIN = CSN_PIN; 
    m_radioNumber = radioNumber;  
}

// init() member function
void RF24Transceiver::init()
{
    // Initializes RF24 class member
    m_radio = new RF24(m_CE_PIN, m_CSN_PIN); 
    m_radio->begin(); 
    m_radio->setPALevel(RF24_PA_LOW); 
    m_radio->setDataRate(RF24_250KBPS); 
    m_radio->enableDynamicPayloads();    
    m_radio->enableAckPayload();
    m_radio->openWritingPipe(address[!m_radioNumber]);
    m_radio->openReadingPipe(1, address[m_radioNumber]);
    m_radio->startListening(); 
}

// transmit() member function
void RF24Transceiver::transmit(String message)
{
    m_radio->stopListening(); 
    bool report = m_radio->write(&message, sizeof(message));
    if (report){    // Checks if message was delivered
        Serial.print(F("Transmission successful! "));           // message was delivered
    } else {
        Serial.println(F("Transmission failed or timed out"));  // payload was not delivered
    }
}

// receive() member function
void RF24Transceiver::receive()
{
    String message;
    uint8_t pipe; 
    if (m_radio->available(&pipe)){                             // Check if transmitter is sending message
        m_radio->read(&message, sizeof(message));               // Read message
        Serial.println(message);                                // Print message
        m_radio->writeAckPayload(1, &message, sizeof(message)); // Send acknowledge payload
    } 
}