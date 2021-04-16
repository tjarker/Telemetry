#include <Arduino.h>
#include <SPI.h>
#include "RF24.h"

// TX/RX byte addresses
static const byte address[][6] = {"00001", "00002"};

// RF24Transceiver class declaration
class RF24Transceiver 
{
private: 
    int m_CE_PIN, m_CSN_PIN; // CE and CSN pins on Teensy 3.6/4.0
    bool m_radioNumber; 
    RF24* m_radio; 

public: 
    RF24Transceiver(int CE_PIN, int CSN_PIN, bool radioNumber);
    void init();
    void transmit(String message);
    void receive(); 
};
