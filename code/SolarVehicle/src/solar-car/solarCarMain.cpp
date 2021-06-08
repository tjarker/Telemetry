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
#include "solar-car/RfTxThd.h"
#include "solar-car/SystemThd.h"


Fifo<CanTelemetryMsg> canFifo(64);
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
  ACANSettings settings(125 * 1000);
  if(ACAN::can0.begin(settings) != 0){Serial.println("CAN setup failed!");}

  // setup radio module
  RFinit();

  bb.init();

  canFifo.clear();

  Serial.println("Starting...");

  chBegin(chSetup);
}

void chSetup(){

  chSysInit();
  
  // create the three worker threads
  BlackboxWorkerBundle blackBoxWorkerBundle = {.fifo = &canFifo, .state = &blackBoxWorkerState, .bb = &bb};
  chThdCreateStatic(blackBoxWorker, sizeof(blackBoxWorker), NORMALPRIO + 3, canWorkerFunc, &blackBoxWorkerBundle);
  rfTxWorkerBundle rfWorkerBundle = {.fifo = &canFifo, .state = &rfWorkerState};
  chThdCreateStatic(waRfWorker,sizeof(waRfWorker),NORMALPRIO + 2, rfWorker, &rfWorkerBundle);
  systemThdBundle systemThdBundle = {.canReceiverState = &canReceiverState, .blackBoxWorkerState = &blackBoxWorkerState, .rfWorkerState = &rfWorkerState};
  chThdCreateStatic(waSystemThd, sizeof(waSystemThd), NORMALPRIO + 1, systemThd, &systemThdBundle);
  CanReceiverBundle canReceiverBundle = {.fifo = &canFifo, .state = &canReceiverState};
  chThdCreateStatic(waCanReceiver, sizeof(waCanReceiver), NORMALPRIO + 1, canReceiverThd, &canReceiverBundle);
  
}
void loop(){}

