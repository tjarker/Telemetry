#include <Arduino.h>
#include <SPI.h>
#include "support-vehicle/threads.h"

// ChibiOS setup function
// Initializes all 4 threads with a working area, priority level, thread function and initial argument. 
void chSetup()
{
  chSysInit();  // Initializes ChibiOS system
  chThdCreateStatic(waRadioWorkerThread, sizeof(waRadioWorkerThread), NORMALPRIO + 1, radioWorkerThread, &sec);
  chThdCreateStatic(waSerialWorkerThread, sizeof(waSerialWorkerThread), NORMALPRIO + 1, serialWorkerThread, &sec);
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
  /*
  for (int i = 0; i < 256; i++){
    uint8_t message = i; 
    Serial.print("Original message: "); 
    Serial.print(message); 
    sec.encrypt(&message, 1); 
    Serial.print("; Encrypted message: ");
    Serial.print(message); 
    sec.decrypt(&message, 1); 
    Serial.print("; Decrypted message: ");
    Serial.print(message); 
    Serial.println();
    delay(2000); 
  }
  /*Serial.println(radio.available()); 
  delay(1000);
  /* Not used */
}