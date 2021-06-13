#ifndef __RF_THD_H__
#define __RF_THD_H__        

#include <ChRt.h>
#include "RFfunctions.h"
#include "ThreadState.h"
#include "Encryption.h"

Security sec; 
ThreadState rfState;

struct threadBundle
{
    Security *sec; 
    ThreadState *state;
};

// 2048 byte working stack for radioWorkerThread
THD_WORKING_AREA(waRadioWorkerThread, 2048);

/**
 * @brief Thread function for radioWorkerThread
 * @param arg, typecast to threadBundle pointer
*/
THD_FUNCTION(radioWorkerThread, arg)
{
  threadBundle *bundle = (threadBundle*)arg; 
  Security *sec = bundle->sec; 
  ThreadState *state = bundle->state; 
  BaseTelemetryMsg received; 
  chThdSleepMicroseconds(100); 

  while (!state->terminate){

    if (state->pause){
      state->suspend(); 
    }

    if (radio.available()){
      if (RFreceive(&received, 32)){
        Serial.print("Received: ");
        //sec->decrypt((uint8_t*)&received, 32);    // Decrypt received message
        char str[256];
        switch (received.cmd){
          case RECEIVED_CAN:{
            CanTelemetryMsg *ptr = (CanTelemetryMsg*)&received; 
            ptr->toJSON(str, sizeof(str)); 
            Serial.println(str); 
          }
            break;
          default:{
            received.toString(str, sizeof(str)); 
            Serial.println(str);
          }
            break;
        }  
      } else {
        Serial.println("Could not receive message.");
      }
    }
    chThdYield();
  }
}

// 2048 byte working stack for serialWorkerThread
THD_WORKING_AREA(waSerialWorkerThread, 2048);

/**
 * @brief Thread function for serialWorkerThread  
 * @param arg, typecast to Security class pointer
*/
THD_FUNCTION(serialWorkerThread, arg)
{
  Security *sec = (Security*)arg; 
  BaseTelemetryMsg message; 

  while (true){                                 
    if (Serial.available()){                      // Check if bytes are available on the serial interface
      Serial.readBytes((char*)&message, 32);      // Read 32 bytes and cast them to a BaseTelemetryMessage 
      //sec->encrypt((uint16_t*)&message, 32);    // Encrypt message before transmission
      if (RFtransmit(&message, 32)){              // Check if message was transmitted succesfully 
        char str[128]; 
        message.toString(str, sizeof(str));
        Serial.print("Transmitted: "); 
        Serial.println(str); 
      } else {      
        Serial.println("Transmission failed or timed out.");  // Message was not transmitted
      }
    }  
    chThdYield();                                 // Yield for same-priority thread                       
  }
} 

#endif