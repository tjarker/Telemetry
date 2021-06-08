#include <Arduino.h>
#include <SPI.h>
#include "RFfunctions.h"

BaseTelemetryMsg msg; 

void setup()
{
    Serial.begin(9600); 
    RFinit(); 
}

void loop()
{
    msg.randomize(); 
    RFtransmit(&msg, 32);
    delay(1000);
}