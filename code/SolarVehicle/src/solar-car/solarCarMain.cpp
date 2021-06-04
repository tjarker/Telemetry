/**
 * This file contains the main function for the solar car unit.
 * 
 * It sets up three worker threads:
 *  - the CAN receiver worker, responsible for putting newly received messages in a FIFO
 *  - the blackbox worker, responsible for adding messages from the FIFO to the logs
 *  - the radio worker, responsible for streaming messages from the FIFO to the support vehicle
 * 
 * The main thread is responsible for listing for commands from the support vehicle and executing them.
 * 
 */

#include <Arduino.h>
#include <ACAN.h>
#include <ChRt.h>

#include "BlackBox.h"
#include "RFfunctions.h"
#include "TelemetryMessages.h"
#include "Fifo.h"
#include "ThreadState.h"

#include "solar-car/blackBoxThd.h"
#include "solar-car/CanReceiverThd.h"
#include "solar-car/Mutexes.h"
#include "solar-car/RfThd.h"

/**
 * Singleton containing global state variables
 */
class GlobalState {
  public:
    uint8_t isLogging = true;
    uint8_t isTransmittingRT = true;
} globalState;

Fifo<CanTelemetryMsg> canFifo(32);
BlackBox bb;
ThreadState blackBoxWorkerState;
ThreadState canReceiverState;
ThreadState rfWorkerState;

void chSetup();
void setup(){

  // initialize serial port
  Serial.begin(9600);
  while(!Serial){} //needs to be removed when headless!!!!!!!!!!!!!!!!!!!!

  // setup built in LED
  pinMode(LED_BUILTIN,OUTPUT);

  Serial.println("Initializing...");

  // setup CAN bus
  //ACANSettings settings(125 * 1000);
  //if(ACAN::can0.begin(settings)){Serial.println("CAN setup failed!");}

  // setup radio module
  RFinit();

  canFifo.clear();

  Serial.println("Starting...");  

  chBegin(chSetup);

}

void chSetup(){

  chSysInit();
  
  // create the three worker threads
  BlackboxWorkerBundle blackBoxWorkerBundle = {.fifo = &canFifo, .state = &blackBoxWorkerState, .bb = &bb};
  chThdCreateStatic(blackBoxWorker, sizeof(blackBoxWorker), NORMALPRIO+1, canWorkerFunc, &blackBoxWorkerBundle);
  rfWorkerBundle rfWorkerBundle = {.fifo = &canFifo, .state = &rfWorkerState};
  chThdCreateStatic(waRfWorker,sizeof(waRfWorker),NORMALPRIO + 1, rfWorker, &rfWorkerBundle);
  CanReceiverBundle canReceiverBundle = {.fifo = &canFifo, .state = &canReceiverState};
  chThdCreateStatic(waCanReceiver, sizeof(waCanReceiver), NORMALPRIO+2, canReceiverThd, &canReceiverBundle);
}

void loop(){

  if(Serial.available()){
    uint8_t read = Serial.read();
    switch (read){
      case 13:
        Serial.println("\n\nStopping...");
        blackBoxWorkerState.terminate = true;
        canReceiverState.terminate = true;
        rfWorkerState.terminate = true;
        while(true){}
        break;

      case 'p':
        if(!canReceiverState.pause){
          Serial.println("Pausing...");
          canReceiverState.pause = true;
        } else {
          Serial.println("Resuming...");
          canReceiverState.wakeUp();
        }
        break;
      default:
        break;
    }
  }
  
}