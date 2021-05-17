//#include "ChUtil.h"
#include "Fifo.h"
#include "RFfunctions.cpp"

// Declare fifoBuffer for RXthread and TXthread 
Fifo<CanTelemetryMsg> RFinbox(32), RFoutbox(32);

// 512 byte working stack for RXthread
THD_WORKING_AREA(waRXthread, 512); 

THD_FUNCTION(RXthread, arg)
{
  (void)arg; 
  radio.startListening(); 
  while (true){
    uint8_t pipe; 
    if (radio.available(&pipe)){
      radio.read(RFoutbox.fifoTail(), 32);                // Always write data to fifoTail
      Serial.println("Message received");
      radio.writeAckPayload(1, RFoutbox.fifoTail(), 32);      
      RFoutbox.fifoMoveTail();                            // Increment fifoTail index
      RFoutbox.signalWrite();                             // Signal data write
    } else {
      Serial.println("No message received");
    }
  }
}

// 512 byte working stack for TXthread
THD_WORKING_AREA(waTXthread, 512);

THD_FUNCTION(TXthread, arg)
{
  (void)arg; 
  radio.stopListening();
  while (true){
    bool report = radio.write(RFinbox.fifoHead(), 32);    // Always read data to fifoHead
    if (report){                                          // Message transmission success
      Serial.print(F("Transmission successful! "));       
      if (radio.isAckPayloadAvailable()){                 // Get ack payload if available
        radio.read(RFinbox.fifoHead(), 32); 
        Serial.print(F("Acknowledge received: "));
        char str[64];
        RFinbox.fifoTail()->toString(str,sizeof(str));    
        Serial.print(str);                                // Print ack payload
      } 
      Serial.println(); 
      RFinbox.fifoMoveHead();                             // Increment fifoHead index
      RFinbox.signalRead();                               // Signal data read
    } else {                                              // Message transmission failed
      Serial.println(F("Transmission failed or timed out"));  
    }
  }
}

void chSetup()
{
  // Initialize RXthread and TXthread with NORMALPRIO + 1
  chThdCreateStatic(waRXthread, sizeof(waRXthread), NORMALPRIO + 1, RXthread, NULL);
  chThdCreateStatic(waTXthread, sizeof(waTXthread), NORMALPRIO + 1, TXthread, NULL);
}

void setup()
{
  Serial.begin(9600);
  while(!Serial){}    // wait for Serial availability
  RFinit();           // setup RF module
  chBegin(chSetup);   // setup threads
  while(true){}      
}

void loop()
{
  /* Not used */
}
