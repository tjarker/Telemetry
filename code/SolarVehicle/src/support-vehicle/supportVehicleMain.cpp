#include <Arduino.h>
#include <SPI.h>
#include "support-vehicle/RfThd.h"

// ChibiOS setup function
// Initializes all 4 threads with a working area, priority level, thread function and initial argument. 
void chSetup()
{
  chSysInit();  // Initializes ChibiOS system
  // Thread arguments are passed as bundles consisting of pointers to a FIFO buffer and the appropriate thread state. 
  /* Old threads
  threadBundle radioTransmitterBundle = {.fifo = &RFinbox, .state = &radioTransmitterState};
  chThdCreateStatic(waRadioTransmitterThread, sizeof(waRadioTransmitterThread), NORMALPRIO + 2, radioTransmitterThread, &radioTransmitterBundle);
  threadBundle serialTransmitterBundle = {.fifo = &RFoutbox, .state = &serialTransmitterState};
  chThdCreateStatic(waSerialTransmitterThread, sizeof(waSerialTransmitterThread), NORMALPRIO + 2, serialTransmitterThread, &serialTransmitterBundle);
  threadBundle radioReceiverBundle = {.fifo = &RFoutbox, .state = &radioReceiverState};
  chThdCreateStatic(waRadioReceiverThread, sizeof(waRadioReceiverThread), NORMALPRIO + 1, radioReceiverThread, &radioReceiverBundle);
  threadBundle serialReceiverBundle = {.fifo = &RFinbox, .state = &serialReceiverState};
  chThdCreateStatic(waSerialReceiverThread, sizeof(waSerialReceiverThread), NORMALPRIO + 1, serialReceiverThread, &serialReceiverBundle);
  */
  // New threads
  threadBundle radioWorkerBundle = {.fifo = {&RFoutbox, &RFinbox}, .state = &radioWorkerState}; 
  chThdCreateStatic(waRadioWorkerThread, sizeof(waRadioWorkerThread), NORMALPRIO + 1, radioWorkerThread, &radioWorkerBundle);
  threadBundle serialWorkerBundle = {.fifo = {&RFoutbox, &RFinbox}, .state = &serialWorkerState}; 
  chThdCreateStatic(waSerialWorkerThread, sizeof(waSerialWorkerThread), NORMALPRIO + 1, serialWorkerThread, &serialWorkerBundle);
    
}

CanTelemetryMsg buf;  

void setup()
{
  Serial.begin(921600); // initialize serial port
  while(!Serial){}
  RFinit(); 
  RFoutbox.clear();
  RFinbox.clear();
  //chBegin(chSetup);     // Initialize and start all 4 threads
  //while (true){}
}

void loop()
{ 
  Serial.readBytes((char*)&buf, sizeof(buf)); 
  char str[64]; 
  buf.toJSON(str,sizeof(str));
  Serial.println(str); 
  delay(1000);
  /* Not used */
}