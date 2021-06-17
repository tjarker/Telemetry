/****************************************************************************************************
 * @file    supportVehicleMain.cpp                                                                  *
 * @author  Steffan Martin Kunoy                                                                    *
 * @brief   Includes setup() and loop() functions to be run on the Support Vehicle microcontroller. *
 ****************************************************************************************************/

#include <Arduino.h>
#include "telemetry/support-vehicle/threads.h"

/*********************************************************
 * @brief Initializes all threads with a working area,   *
 * priority level, thread function and initial argument. *
 *********************************************************/  
void chSetup()
{
  chSysInit();          // Initializes ChibiOS system
  chThdCreateStatic(WaReceiverThread, sizeof(WaReceiverThread), NORMALPRIO + 1, receiverThread, &sec);
  threadBundle transmitterBundle = {.security = &sec, .fifo = &TXfifo}; 
  chThdCreateStatic(waTransmitterThread, sizeof(waTransmitterThread), NORMALPRIO + 1, transmitterThread, &transmitterBundle);
}

void setup()
{
  Serial.begin(921600); // Initialize serial port
  while (!Serial){}     // Wait until serial is available
  if (!RFinit()){       // Initialize and configure RF24 module
    Serial.println("Radio not working!"); 
    radio.printDetails(); 
  }
  sec.encryption_key(); // Create message encryption key
  TXfifo.clear();       // Clears TXfifo 
  chBegin(chSetup);     // Initialize and start all 4 threads
  while (true){}        
}

void loop()
{
  /* Not used */
}