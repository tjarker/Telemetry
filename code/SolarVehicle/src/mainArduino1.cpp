#include "RFfunctions.cpp"

void setup() 
{
  Serial.begin(9600);
  RFinit();
}

void loop() 
{
  CanTelemetryMsg msg;
  RFtransmit(msg.toMessage(), sizeof(msg));
  delay(1000);
}