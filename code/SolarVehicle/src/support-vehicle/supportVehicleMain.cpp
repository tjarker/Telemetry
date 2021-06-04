/**
 * This file contains the main function for the solar car unit.
 * 
 * It sets up three worker threads:
 *  - the CAN receiver worker, responsible for putting newly received messages in a FIFO
 *  - the blackbox worker, responsible for adding messages from the FIFO to the logs
 *  - the radio worker, responsible for streaming messages from the FIFO to the support vehicle
 * 
 * The main thread is responsible for listing for commands from the support vehicle and executing them.
 * 
 */

#include <Arduino.h>
#include "RFfunctions.h"
#include "TelemetryMessages.h"
#include "support-vehicle/RfThd.h"

Fifo<BaseTelemetryMsg> RFoutbox(32), RFinbox(32); 
ThreadState RXthreadState, TXthreadState; 

void chSetup(){

  chSysInit();
  rfWorkerBundle RXworkerBundle = {.fifo = &RFoutbox, .state = &RXthreadState};
  chThdCreateStatic(waRXthread,sizeof(waRXthread), NORMALPRIO + 1, RXthread, &RXworkerBundle);
  rfWorkerBundle TXworkerBundle = {.fifo = &RFinbox, .state = &TXthreadState};
  chThdCreateStatic(waRXthread,sizeof(waTXthread), NORMALPRIO + 1, TXthread, &TXworkerBundle);
}

void setup()
{
  Serial.begin(9600); // initialize serial port
  RFinit(); 
}

void loop()
{
  
}