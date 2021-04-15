#include "TeensyTransceiver.h"

// RF24Transceiver Constructor
RF24Transceiver::RF24Transceiver(int CE_PIN, int CSN_PIN, bool radioNumber)
{
    init(CE_PIN, CSN_PIN, radioNumber); 
}

// init() member function
void RF24Transceiver::init(int CE_PIN, int CSN_PIN, bool radioNumber)
{
    m_CE_PIN = CE_PIN;
    m_CSN_PIN = CSN_PIN; 
    m_radioNumber = radioNumber; 
    // Initializes RF24 class object
    m_radio = new RF24(CE_PIN, CSN_PIN); 
    m_radio->begin(); 
    m_radio->setPALevel(RF24_PA_LOW); 
    m_radio->enableDynamicPayloads();    
    m_radio->enableAckPayload();
    m_radio->openWritingPipe(address[!m_radioNumber]);
    m_radio->openReadingPipe(1, address[m_radioNumber]);
}

void RF24Transceiver::standby()
{
    
}