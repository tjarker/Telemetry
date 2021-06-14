#include <Arduino.h>
#include <SPI.h>
#include "support-vehicle/threads.h"
#include "RSA.h"


void rsa_print1()
{
  Security sec; 
  BaseTelemetryMsg msg; char str[64]; 
  msg.randomize(); 
  msg.toString(str, sizeof(str)); 
  Serial.println(str); 
  sec.encrypt((uint16_t*)&msg, 32); 
  msg.toString(str, sizeof(str)); 
  Serial.println(str);
  sec.decrypt((uint16_t*)&msg, 32); 
  msg.toString(str, sizeof(str)); 
  Serial.println(str);
}

void rsa_print2()
{
  RSA rsa; 
  BaseTelemetryMsg msg; char str[64]; 
  msg.randomize(); 
  msg.toString(str, sizeof(str)); 
  Serial.println(str); 
  rsa.encrypt((uint8_t*)&msg, 32); 
  msg.toString(str, sizeof(str)); 
  Serial.println(str);
  rsa.decrypt((uint8_t*)&msg, 32); 
  msg.toString(str, sizeof(str)); 
  Serial.println(str);
}

// ChibiOS setup function
// Initializes all 4 threads with a working area, priority level, thread function and initial argument. 
void chSetup()
{
  chSysInit();  // Initializes ChibiOS system
  threadBundle bundle = {.sec = &sec, .state = &rfState}; 
  chThdCreateStatic(WaReceiverThread, sizeof(WaReceiverThread), NORMALPRIO + 1, receiverThread, &bundle);
  chThdCreateStatic(waTransmitterThread, sizeof(waTransmitterThread), NORMALPRIO + 1, transmitterThread, &sec);
}  

void setup()
{
  Serial.begin(921600); // Initialize serial port
  while(!Serial){}      // Wait until serial is available
  RFinit();             // Initialize RF module
  chBegin(chSetup);     // Initialize and start all 4 threads
  while (true){}        
}

void loop()
{
  /* Not used */
}