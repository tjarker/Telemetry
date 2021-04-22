#include "TeensyTransceiver.h"

// RF24Transceiver() Constructor member function
RF24Transceiver::RF24Transceiver(int CE_PIN, int CSN_PIN, bool radioNumber)
{
    init(CE_PIN, CSN_PIN, radioNumber); 
}

// init() member function
void RF24Transceiver::init(int CE_PIN, int CSN_PIN, bool radioNumber)
{
    // Assigns member variables according to constructor arguments
    m_CE_PIN = CE_PIN;
    m_CSN_PIN = CSN_PIN; 
    m_radioNumber = radioNumber; 

    // Initializes RF24 class member
    m_radio = new RF24(CE_PIN, CSN_PIN); 
    m_radio->begin(); 
    m_radio->setPALevel(RF24_PA_LOW); 
    m_radio->setDataRate(RF24_250KBPS); 
    m_radio->enableDynamicPayloads();    
    m_radio->enableAckPayload();
    m_radio->openWritingPipe(address[!m_radioNumber]);
    m_radio->openReadingPipe(1, address[m_radioNumber]);
    m_radio->startListening(); 
}

// standby() member function
void RF24Transceiver::standby()
{
    if (!write_buffer.empty()){
        // When write_buffer contains data switch to TX mode
        m_radio->stopListening(); 
        while (!write_buffer.empty()){
            // Transmit data until write_buffer is empty
            // Prepare payload message by popping it from write_buffer 
            uint32_t payload = write_buffer.pop_back(); 
            unsigned long start_timer = micros();                       // start the timer
            bool report = m_radio->write(&payload, sizeof(payload));    // transmit & save the report
            unsigned long end_timer = micros();                         // end the timer
            if (report){
                // Checks if payload was delivered
                Serial.print(F("Transmission successful! "));               // payload was delivered
                Serial.print(F("Time to transmit = "));
                Serial.print(end_timer - start_timer); 
                Serial.println(); 
                uint8_t pipe; 
                if (m_radio->available(&pipe)){
                    Serial.println(F(" Recieved "));
                } else {
                    Serial.println(F(" Recieved: an empty ACK packet"));   // empty ACK packet received
                }
            } else {
                Serial.println(F("Transmission failed or timed out"));     // payload was not delivered
            }
        }  
    } else {
        // Switch to RX mode as long as other antenna is sending data 
        uint8_t pipe;
        while(m_radio->available(&pipe)){
            
        }
    }
}