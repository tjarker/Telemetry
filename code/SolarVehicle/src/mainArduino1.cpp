#include "RFfunctions.cpp"

void setup() 
{
  Serial.begin(9600);
  RFinit();
}

void loop() 
{
  TelemetryMessage msg = TelemetryMessage();
  RFtransmit(msg, sizeof(msg));
  delay(1000);
}