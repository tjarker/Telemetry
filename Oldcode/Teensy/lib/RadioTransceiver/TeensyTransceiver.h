#include <Arduino.h>
#include <SPI.h>
#include <Array.h>
#include "RF24.h"

static const byte address[][6] = {"NodeA", "NodeB"};
const int BUFFER_SIZE = 1000; 
Array<uint32_t, BUFFER_SIZE> write_buffer;  // FIFO CAN data buffer to transmit
Array<uint32_t, BUFFER_SIZE> read_buffer;   // FIFO CAN data buffer to receive

class RF24Transceiver 
{
private: 
    int m_CE_PIN, m_CSN_PIN; // CE and CSN pins on Teensy 3.6/4.0
    bool m_radioNumber; 
    RF24* m_radio; 

public: 
    RF24Transceiver(int CE_PIN, int CSN_PIN, bool radioNumber);
    void init(int CE_PIN, int CSN_PIN, bool radioNumber);
    void standby();
};
