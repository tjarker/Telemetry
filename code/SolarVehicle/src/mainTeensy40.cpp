#include "ChUtil.h"
#include "RFfunctions.cpp"

// Declare fifoBuffer for RXthread and TXthread 
fifoBuffer RFoutbox, RFinbox;

// 512 byte working stack for RXthread
THD_WORKING_AREA(waRXthread, 512); 

THD_FUNCTION(RXthread, arg)
{
  (void)arg; 
  radio.startListening(); 
  while (true){
    uint8_t pipe; 
    if (radio.available(&pipe)){
      radio.read(RFoutbox.fifoTail(), 32);
      Serial.println("Message received");
      radio.writeAckPayload(1, RFoutbox.fifoTail(), 32);      
      RFoutbox.fifoMoveTail();
      chSemSignal(&RFoutbox.fifoData);
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
    bool report = radio.write(RFinbox.fifoHead(), 32);
    if (report){
      Serial.print(F("Transmission successful! "));
      if (radio.isAckPayloadAvailable()){
        radio.read(RFinbox.fifoHead(), 32); 
        Serial.print(F("Acknowledge received: "));
        char str[64];
        RFinbox.fifoHead()->toString(str,sizeof(str));
        Serial.print(str);
      }
      Serial.println(); 
      RFinbox.fifoMoveTail();
      chSemSignal(&RFinbox.fifoSpace);     
    } else {
      Serial.println(F("Transmission failed or timed out"));  // message was not delivered
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
