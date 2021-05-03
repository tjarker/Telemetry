#include "RFfunctions.cpp"

void setup()
{
  Serial.begin(9600);
  RFinit();
}

void loop()
{
  BaseTelemetryMsg received; 
  RFreceive(&received);
}