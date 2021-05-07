
#include "RFfunctions.cpp"

void setup()
{
  Serial.begin(9600);
  while (!Serial){}
  RFinit();
}

void loop()
{
  BaseTelemetryMsg *received; 
  RFreceive(received);
}
/*
#include "ChRt.h"
#include "RFfunctions.cpp"

#define NUM_BUFFERS 4

static BaseTelemetryMsg RFinmessages[NUM_BUFFERS]; 
static BaseTelemetryMsg RFoutmessages[NUM_BUFFERS];

MAILBOX_DECL(RFinbox, &RFinmessages, NUM_BUFFERS); 
MAILBOX_DECL(RFoutbox, &RFoutmessages, NUM_BUFFERS);

THD_WORKING_AREA(waRXthread, 512); 
THD_WORKING_AREA(waTXthread, 512);

THD_FUNCTION(RXthread, arg)
{
  (void)arg; 
  while (true){
    
  }
}

THD_FUNCTION(TXthread, arg)
{
  (void)arg; 
  while (true){

  }
}

void chSetup()
{
  chThdCreateStatic(waRXthread, sizeof(waRXthread), NORMALPRIO + 1, RXthread, NULL);
  chThdCreateStatic(waTXthread, sizeof(waTXthread), NORMALPRIO + 1, TXthread, NULL);
}

void setup()
{
  Serial.begin(9600);
  while(!Serial){}
  RFinit();
  chBegin(chSetup);
  while(true){}
}

void loop(){}
*/