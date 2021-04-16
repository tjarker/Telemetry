#include "RF24Transceiver.h"

// Initialize RF24Transceiver object
RF24Transceiver radio(9, 10, 0);    // CE pin, CSN pin, radioNumber (0 or 1)

void setup()
{
    Serial.begin(9600); 
    while(!Serial){}
}

void loop()
{
    radio.receive();    // Should print received message
}