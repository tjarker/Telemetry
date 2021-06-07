#include <Arduino.h>
#include <SPI.h>
#include "support-vehicle/RfThd.h"

Fifo<BaseTelemetryMsg> RFoutbox(32), RFinbox(32); 
ThreadState radioReceiverState, radioTransmitterState, serialReceiverState, serialTransmitterState; 

void chSetup()
{
  chSysInit();
  threadBundle radioReceiverBundle = {.fifo = &RFinbox, .state = &radioReceiverState};
  chThdCreateStatic(waRadioReceiverThread, sizeof(waRadioReceiverThread), NORMALPRIO + 1, radioReceiverThread, &radioReceiverBundle);
  threadBundle radioTransmitterBundle = {.fifo = &RFoutbox, .state = &radioTransmitterState};
  chThdCreateStatic(waRadioTransmitterThread, sizeof(waRadioTransmitterThread), NORMALPRIO + 2, radioTransmitterThread, &radioTransmitterBundle);
  threadBundle serialReceiverBundle = {.fifo = &RFoutbox, .state = &serialReceiverState};
  chThdCreateStatic(waSerialReceiverThread, sizeof(waSerialReceiverThread), NORMALPRIO + 1, serialReceiverThread, &serialReceiverBundle);
  threadBundle serialTransmitterBundle = {.fifo = &RFinbox, .state = &serialTransmitterState};
  chThdCreateStatic(waSerialTransmitterThread, sizeof(waSerialTransmitterThread), NORMALPRIO + 2, serialTransmitterThread, &serialTransmitterBundle); 
}

void setup()
{
  Serial.begin(9600); // initialize serial port
  RFinit(); 
  RFoutbox.clear();
  RFinbox.clear();
  chBegin(chSetup);
}

void loop()
{
  /* Not used */
}