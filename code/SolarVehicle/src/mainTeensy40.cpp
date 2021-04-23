#include "RF24Transceiver.h"

RF24Transceiver radio(9, 10, 0); // CE pin, CSN pin, radioNumber (0 or 1)

void setup() 
{
  Serial.begin(9600);
  radio.init();
}

void loop() 
{
  StampedCANMessage received;
  radio.receive(received); 
  Serial.println(received.toString()); 
}