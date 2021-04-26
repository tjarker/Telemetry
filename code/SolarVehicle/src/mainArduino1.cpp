#include "RFfunctions.cpp"

void setup() 
{
  Serial.begin(9600);
  RFinit();
}

void loop() 
{
  // char text[] = "Hello"; 
  // RFtransmit((const void*)text, sizeof(text));
  // PayloadStruct msg = {"Bueno!", 0};
  RFtransmit();
  delay(1000);
}