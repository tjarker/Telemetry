#define ARDUINOCODE
#include "RF24Transceiver.h"

RF24Transceiver radio(9, 10, 1); // CE pin, CSN pin, radioNumber (0 or 1)

void setup() 
{
  Serial.begin(9600); 
  radio.init();
}

void loop() 
{
  char message[] = "Hello Solar Vehicle, copy?";
  radio.transmit(*message, sizeof(message));
  delay(1000);
}