#ifndef __RF_THD_H__
#define __RF_THD_H__        

#include <ChRt.h>
#include "RFfunctions.h"
#include "Encryption.h"

Security sec; 

THD_WORKING_AREA(waRadioWorkerThread, 2048);

THD_FUNCTION(radioWorkerThread, arg)
{ 
  Security *sec = (Security*)arg;
  BaseTelemetryMsg received; 
  chThdSleepMicroseconds(100); 

  while (true){
    
    if (radio.available()){
      if (RFreceive(&received, 32)){
        Serial.print("Received: ");
        //sec->decrypt((uint8_t*)&received, 32);    // Decrypt received message
        char str[256];
        Serial.println((uint8_t) received.cmd); 
        switch (received.cmd){
          case RECEIVED_CAN:
            CanTelemetryMsg *ptr = (CanTelemetryMsg*)&received; 
            ptr->toJSON(str, sizeof(str)); 
            Serial.println(str); 
            break;
          default:
            Serial.println("default"); 
            received.toString(str, sizeof(str)); 
            Serial.println(str); 
            break; 
        }  
      } else {
        Serial.println("Could not receive message.");
      }
    }
    chThdYield();
  }
}

THD_WORKING_AREA(waSerialWorkerThread, 2048);

THD_FUNCTION(serialWorkerThread, arg)
{
  Security *sec = (Security*)arg; 
  BaseTelemetryMsg message; 

  while (true){

    if (Serial.available()){
      Serial.readBytes((char*)&message, 32);
      //sec->encrypt((uint8_t*)&message, 32); 
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