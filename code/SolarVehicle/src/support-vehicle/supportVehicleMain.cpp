/****************************************************************************************************
 * @file    supportVehicleMain.cpp                                                                  *
 * @author  Steffan Martin Kunoy                                                                    *
 * @brief   Includes setup() and loop() functions to be run on the support vehicle microcontroller. *
 ****************************************************************************************************/

#include <Arduino.h>
#include <SPI.h>
#include "support-vehicle/threads.h"
#include "RSA.h"

/*********************************************************
 * @brief Initializes all threads with a working area,   *
 * priority level, thread function and initial argument. *
 *********************************************************/  
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