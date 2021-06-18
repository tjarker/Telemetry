/**************************************************************
 * @file    SystemThds.h                                      *
 * @author  Steffan Martin Kunoy                              *
 * @brief   ChibiOS threads to be run on the Support Vehicle. *
 **************************************************************/

#ifndef __RF_THD_H__
#define __RF_THD_H__        

#include <ChRt.h>
#include "RFfunctions.h"
#include "TelemetryMessages.h"
#include "ThreadState.h"
#include "Fifo.h"
#include "Encryption.h"

Security sec;                       // Security global variable
Fifo<BaseTelemetryMsg> TXfifo(32);  // Fifo global variable

// A struct used to pass arguments to transmitterThd
struct thdBundle 
{
  Security *security;             // Security class member
  Fifo<BaseTelemetryMsg> *fifo;   // Fifo class member
};

// 2048 byte working stack for receiverThd
THD_WORKING_AREA(WaReceiverThd, 2048);

/*****************************************************
 * @brief   Thread function for receiverThd.      *
 * @param   arg, typecast to Security class pointer. *
******************************************************/
THD_FUNCTION(receiverThd, arg)
{
  Security *sec = (Security*)arg;                                   // Cast input argument to Security class pointer
  BaseTelemetryMsg received;                                        // BaseTelemetryMsg object for received messages
  uint16_t encrypted[16];                                           // Buffer to contain encrypted message
  char tempStr[256];                                                // Buffer to print received message to serial
  chThdSleepMicroseconds(100);                                        // Sleep to allow for creation of secondary thread

  while (true){
  
    if (radio.available()){                                           // Check if TX node is transmitting data
      if (RFreceive(encrypted, BaseTelemetryMsg::length()<<1)){       // Check if message was received
        Serial.print("Received: ");
        sec->decrypt(encrypted, (uint8_t*)&received, 16);             // Decrypt received message
        switch (received.cmd){                                        // Switch based on the message cmd field 
          case RECEIVED_CAN:{                                         // Standard case, cmd matches RECEIVED_CAN
            CanTelemetryMsg *ptr = (CanTelemetryMsg*)&received;       // Cast received to CanTelemetryMsg pointer
            ptr->toJSON(tempStr, sizeof(tempStr));                  
            Serial.println(tempStr);                                  // Print message formatted as JSON
          }
            break;
          default:{                                                   // Default case, cmd does not match RECEIVED_CAN
            received.toString(tempStr, sizeof(tempStr));            
            Serial.println(tempStr);                                  // Print message formatted as string
          }
            break;
        }  
      } else {
        Serial.println("Could not receive message.");                 // Message was not received 
      }
    }
    chThdYield();                                                     // Yield for same-priority Thd
  }
}

// 2048 byte working stack for transmitterThd
THD_WORKING_AREA(waTransmitterThd, 2048);

/*****************************************************
 * @brief   Thread function for transmitterThd.   *
 * @param   arg, typecast to thdBundle pointer.   *
******************************************************/
THD_FUNCTION(transmitterThd, arg)
{
  thdBundle *transmitterBundle = (thdBundle*)arg;               // Cast input argument to threadBundle pointer
  Security *transmitterSecurity = transmitterBundle->security;        // Extract bundle Security member variable
  Fifo<BaseTelemetryMsg> *transmitterFifo = transmitterBundle->fifo;  // Extract bundle Fifo member variable
  uint16_t encrypted[16];                                             // Buffer to contain encrypted message
  char tempStr[128];                                                  // Buffer to print message to serial

  while (true){
    if (Serial.available()){                                          // Check if bytes are available on the serial interface
      Serial.readBytes((char*)transmitterFifo->tail(), 32);           // Read 32 bytes and cast them to a BaseTelemetryMessage                           
      transmitterFifo->moveTail();                                    // Increment fifo tail pointer
    }
    if (!transmitterFifo->empty()){                                   // Check if there are messages in the fifo waiting to be sent
      // Choose the message at the head of the fifo and encrypt it before transmission
      transmitterSecurity->encrypt((uint8_t*)transmitterFifo->head(), encrypted, BaseTelemetryMsg::length());    
      if (RFtransmit(encrypted, BaseTelemetryMsg::length()<<1)){      // Check if message was transmitted succesfully 
        (transmitterFifo->head())->toString(tempStr, sizeof(tempStr));
        Serial.print("Transmitted: "); 
        Serial.println(tempStr);                                      // Print transmitted message to serial
        transmitterFifo->moveHead();                                  // Increment fifo head pointer
      } else {      
        Serial.println("Transmission failed or timed out.");          // Message was not transmitted
      }
    }
    chThdYield();                                                     // Yield for same-priority thread                       
  }

} 

#endif