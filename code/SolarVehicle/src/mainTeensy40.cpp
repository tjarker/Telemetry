#include "Fifo.h"
#include "util.h"
#include "RFfunctions.cpp"

// Initialize state machine for RXthread and TXthread
ThreadState radioReceiverThreadState, radioTransmitterThreadState, serialReceiverThreadState, serialTransmitterThreadState; 

// Declare fifoBuffer for RXthread and TXthread 
Fifo<CanTelemetryMsg> RFinbox(32), RFoutbox(32);

// radioReceiverThread working stack
THD_WORKING_AREA(waRXthread, 256); 

// RXthread function thread
THD_FUNCTION(RXthread, arg)
{
  ThreadState *state = (ThreadState*)arg;
  radio.startListening(); 
  while (!state->terminate){
    if(state->pause || RFinbox.fifoFull())                // Suspend thread if: 1. state = pause OR
    { state->suspend(); }                                 //                    2. fifo buffer is full
    uint8_t pipe; 
    if (radio.available(&pipe)){                          // Check for incoming messages
      radio.read(RFoutbox.fifoTail(), 32);                // Read incoming message into fifo
      Serial.println("Message received");                 
      radio.writeAckPayload(1, RFoutbox.fifoTail(), 32);  // Return ack payload to transmitter    
      RFoutbox.fifoMoveTail();                            // Increment fifoTail index
      RFoutbox.signalWrite();                             // Signal data write
    } else {
      Serial.println("No message received");
    }
  }
}

// radioTransmitterThread working stack
THD_WORKING_AREA(waTXthread, 256);

// TXthread function thread
THD_FUNCTION(TXthread, arg)
{
  ThreadState *state = (ThreadState*)arg;  
  radio.stopListening();
  while (!state->terminate){
    if (state->pause || RFinbox.fifoEmpty())                  // Suspend thread if: 1. state = pause OR
    { state->suspend(); }                                     //                    2. fifo buffer is empty
    bool report = radio.write(RFinbox.fifoHead(), 32);        // Attempt data transmission
    if (report){                                              
      Serial.print(F("Transmission successful! "));           // Message transmission success
      if (radio.isAckPayloadAvailable()){                     // Get ack payload if available
        radio.read(RFinbox.fifoHead(), 32);               
        Serial.print(F("Acknowledge received: "));
        char str[64];
        RFinbox.fifoTail()->toString(str,sizeof(str));    
        Serial.print(str);                                    // Print ack payload
      } 
      Serial.println(); 
      RFinbox.fifoMoveHead();                                 // Increment fifoHead index
      RFinbox.signalRead();                                   // Signal data read
    } else {                                                
      Serial.println(F("Transmission failed or timed out"));  // Message transmission failed
    }
  }
}

// serialReceiverThread working stack
THD_WORKING_AREA(waSerialReceiverThread, 256);

// serialTransmitterThread working stack
THD_FUNCTION(serialReceiverThread, arg)
{
  ThreadState *state = (ThreadState*)arg;
  while(!state->terminate){
    
  }
}

THD_WORKING_AREA(waSerialTransmitterThread, 256);

THD_FUNCTION(serialTransmitterThread, arg)
{
  ThreadState *state = (ThreadState*)arg;
  while(!state->terminate){
    
  }
}

// Initialize RXthread and TXthread
void chSetup()
{
  chThdCreateStatic(waRXthread, sizeof(waRXthread), NORMALPRIO + 1, RXthread, &radioReceiverThreadState);
  chThdCreateStatic(waTXthread, sizeof(waTXthread), NORMALPRIO + 1, TXthread, &radioTransmitterThreadState);
  chThdCreateStatic(waSerialReceiverThread, sizeof(waSerialReceiverThread), NORMALPRIO + 1, serialReceiverThread, &serialReceiverThreadState);
  chThdCreateStatic(waSerialTransmitterThread, sizeof(waSerialTransmitterThread), NORMALPRIO + 1, serialTransmitterThread, &serialTransmitterThreadState);
}

// setup() function compiles first
void setup()
{
  Serial.begin(9600);
  while(!Serial){}    // wait for Serial availability
  RFinit();           // setup RF module
  RFinbox.clear();      
  RFoutbox.clear();   // Clear fifo buffers
  chBegin(chSetup);   // setup threads
  while(true){}      
}

// loop() function 
void loop()
{
  /* Not used */
}
