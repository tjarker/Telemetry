#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include "logType.h"

#define COUNT 5     // Number of transmission retries
#define DELAY 15    // Delay between retries (= DELAY * 250 us + 250 us)

// TX/RX byte addresses
static const byte address[][6] = {"00001", "00002"};

// RF24Transceiver class declaration
class RF24Transceiver 
{
    // private member variables
private: 
    int m_CE_PIN, m_CSN_PIN; // CE and CSN pins on Teensy 3.6/4.0
    bool m_radioNumber; 
    RF24* m_radio; 
    // public member functions
public: 
    RF24Transceiver(int CE_PIN, int CSN_PIN, bool radioNumber);
    void init();
    void transmit(char &message, int size);
    void transmit(StampedCANMessage &message);  // Overloaded transmit() function
    void receive(); 
    void receive(StampedCANMessage &message);   // Overloaded receive() function
};
