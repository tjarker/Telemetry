#include "RFfunctions.cpp"

void setup() 
{
  Serial.begin(9600);
  RFinit();
}

void loop() 
{
  StampedCANMessage msg = StampedCANMessage();
  RFtransmit(msg, sizeof(msg));
  delay(1000);
}