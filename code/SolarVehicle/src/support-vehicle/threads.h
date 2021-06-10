#ifndef __RF_THD_H__
#define __RF_THD_H__        

#include <ChRt.h>
#include "RFfunctions.h"

THD_WORKING_AREA(waRadioWorkerThread, 2048);

THD_FUNCTION(radioWorkerThread, arg)
{ 
  (void)arg;
  BaseTelemetryMsg received; 

  while (true){
    
    if (radio.available()){
      if (RFreceive(&received, 32)){
      Serial.print("Received: ");
      cmd_t r = received.cmd; 
      switch (r){
        case RECEIVED_CAN:
          char str[256]; 
          CanTelemetryMsg *ptr = (CanTelemetryMsg*)&received; 
          ptr->toJSON(str, sizeof(str)); 
          Serial.print(str); 
          break;
        default: 
          break; 
      }
      Serial.println();  
      } else {
        Serial.println("Could not receive message.");
      }
    }
    //if (Serial.available()) chThdSleepMilliseconds(100); 
    chThdYield();
  }
}

THD_WORKING_AREA(waSerialWorkerThread, 2048);

THD_FUNCTION(serialWorkerThread, arg)
{
  (void)arg; 
  BaseTelemetryMsg message; 

  while (true){
     
    if (Serial.available()){
      Serial.readBytes((char*)&message, 32);
      if (RFtransmit(&message, 32)){
        char str[64]; 
        message.toString(str, sizeof(str));
        Serial.print("Transmitted: "); 
        Serial.println(str); 
        Serial.println("Acknowledge received.");
      } else {
        Serial.println("Transmission failed or timed out.");
      }
    }  
    chThdYield();  
  }
} 

#endif