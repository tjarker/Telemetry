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

#include "telemetry/solar-car/blackBoxThd.h"
#include "telemetry/solar-car/CanReceiverThd.h"
#include "telemetry/solar-car/Mutexes.h"
#include "telemetry/solar-car/RfTxThd.h"
#include "telemetry/solar-car/SystemThd.h"


#ifdef CH_CFG_TIME_QUANTUM
#undef CH_CFG_TIME_QUANTUM
#define CH_CFG_TIME_QUANTUM 0
#endif

Fifo<CanTelemetryMsg> bbFifo(16), rfFifo(64);
BlackBox bb(500);
ThreadState blackBoxWorkerState;
ThreadState canReceiverState;
ThreadState rfWorkerState;
Security security;

void chSetup();
void setup(){
  
  // initialize serial port
  Serial.begin(921600);
  while(!Serial){} //needs to be removed when headless!!!!!!!!!!!!!!!!!!!!

  // setup built in LED
  pinMode(LED_BUILTIN,OUTPUT);

  Serial.println("System Status:\n");

  // setup CAN bus
  ACANSettings settings(125 * 1000);
  if(ACAN::can0.begin(settings) != 0){
    Serial.println("\tCAN:\t\t Failure");
  } else {
    Serial.println("\tCAN:\t\tOK");
  }

  // setup radio module
  if(RFinit()){
    Serial.println("\tRF:\t\tOK");
  } else {
    Serial.println("\tRF:\t\tFailure");
  }

  if(bb.init()) {
    Serial.println("\tSD card:\tOK");
  } else {
    Serial.println("\tSD card:\tFailure");
  }

  security.encryption_key();

  bbFifo.clear();
  rfFifo.clear();

  Serial.println("\nStarting Threads:\n");

  chBegin(chSetup);
}

void chSetup(){

  chSysInit();

  blackBoxWorkerState.pause = true;
  rfWorkerState.pause = true;
  canReceiverState.pause = false;
  
  // create the three worker threads
  BlackboxWorkerBundle blackBoxWorkerBundle = {.fifo = &bbFifo, .state = &blackBoxWorkerState, .bb = &bb};
  chThdCreateStatic(blackBoxWorker, sizeof(blackBoxWorker), NORMALPRIO + 3, canWorkerFunc, &blackBoxWorkerBundle);
  rfTxWorkerBundle rfWorkerBundle = {.fifo = &rfFifo, .state = &rfWorkerState, .sec = &security};
  chThdCreateStatic(waRfWorker,sizeof(waRfWorker),NORMALPRIO + 2, rfWorker, &rfWorkerBundle);
  systemThdBundle systemThdBundle = {.canReceiverState = &canReceiverState, .blackBoxWorkerState = &blackBoxWorkerState, .rfWorkerState = &rfWorkerState, .sec = &security};
  chThdCreateStatic(waSystemThd, sizeof(waSystemThd), NORMALPRIO + 1, systemThd, &systemThdBundle);
  CanReceiverBundle canReceiverBundle = {.bbFifo = &bbFifo, .rfFifo = &rfFifo, .state = &canReceiverState};
  chThdCreateStatic(waCanReceiver, sizeof(waCanReceiver), NORMALPRIO + 1, canReceiverThd, &canReceiverBundle);
  
}
void loop(){Serial.println("loop!!!!!!!!!!!!!!!!!!!!!!!");}

