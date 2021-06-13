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
  chThdCreateStatic(waRadioWorkerThread, sizeof(waRadioWorkerThread), NORMALPRIO + 1, radioWorkerThread, &bundle);
  chThdCreateStatic(waSerialWorkerThread, sizeof(waSerialWorkerThread), NORMALPRIO + 1, serialWorkerThread, &sec);
}  

void setup()
{
  Serial.begin(921600); // initialize serial port
  while(!Serial){}
  RFinit(); 
  chBegin(chSetup);     // Initialize and start all 4 threads
  //rsa_print2();
  while (true){}
}

void loop()
{
  /*Serial.println(radio.available()); 
  delay(1000);
  /* Not used */
}