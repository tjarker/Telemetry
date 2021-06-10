#include <Arduino.h>
#include <SPI.h>
#include "support-vehicle/threads.h"

// ChibiOS setup function
// Initializes all 4 threads with a working area, priority level, thread function and initial argument. 
void chSetup()
{
  chSysInit();  // Initializes ChibiOS system
  chThdCreateStatic(waRadioWorkerThread, sizeof(waRadioWorkerThread), NORMALPRIO + 1, radioWorkerThread, &sec);
  chThdCreateStatic(waSerialWorkerThread, sizeof(waSerialWorkerThread), NORMALPRIO + 1, serialWorkerThread, NULL);
}  

void setup()
{
  Serial.begin(921600); // initialize serial port
  while(!Serial){}
  RFinit(); 
  chBegin(chSetup);     // Initialize and start all 4 threads
  //while (true){}
}

void loop()
{  
  /*Serial.println(radio.available()); 
  delay(1000);
  /* Not used */
}