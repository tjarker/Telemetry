/*
#include "ChRt.h"
#include "RFfunctions.cpp"

const size_t FIFO_SIZE = 32;

// Count of data records in fifo.
SEMAPHORE_DECL(fifoData, 0);

// Count of free buffers in fifo.
SEMAPHORE_DECL(fifoSpace, FIFO_SIZE);

// Array of fifo items.
BaseTelemetryMsg fifoArray[FIFO_SIZE];

THD_WORKING_AREA(waRXthread, 512); 

THD_FUNCTION(RXthread, arg)
{
  (void)arg; 
  radio.stopListening();
  while (true){
    if(chSemWaitTimeout(&fifoSpace, TIME_IMMEDIATE) != MSG_OK){
      continue;
    }
    
    chSemSignal(&fifoData);
  }
}

THD_WORKING_AREA(waTXthread, 512);

THD_FUNCTION(TXthread, arg)
{
  (void)arg; 
  size_t fifoHead = 0; 

  while (true){
    if(chSemWaitTimeout(&fifoSpace, TIME_IMMEDIATE) != MSG_OK){
      continue;
    }
    BaseTelemetryMsg *msg = &fifoArray[fifoHead]; 
    RFtransmit(msg, 32);

    // Advance FIFO index.
    fifoHead = fifoHead < (FIFO_SIZE - 1) ? fifoHead + 1 : 0;
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
#include "RFfunctions.cpp"

void setup()
{
  Serial.begin(9600);
  while (!Serial){}
  RFinit();
}

void loop()
{
  BaseTelemetryMsg received; 
  RFreceive(&received);
  char tmp[64];
  switch (received.cmd){

    case RECEIVED_CAN: 
      ((CanTelemetryMsg *) &received)->toString(tmp, sizeof(tmp)); 
      Serial.println(tmp);
      break; 

    default:
      break;
  }
}
