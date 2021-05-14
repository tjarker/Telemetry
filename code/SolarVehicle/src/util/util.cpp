#include <Arduino.h>
#include "util.h"



ExecutionTimeMeasurer::ExecutionTimeMeasurer(const char* name) : killed(false){
    this->name = name;
    startTime = micros();
}
ExecutionTimeMeasurer::~ExecutionTimeMeasurer(){
    uint32_t end = micros();
    Serial.printf("%s took %i us\n",name,end-startTime);
}
void ExecutionTimeMeasurer::kill(){
    killed = true;
}
bool ExecutionTimeMeasurer::isKilled(){
    return killed;
}

ActivityMeasurer::ActivityMeasurer() : killed(false){
    digitalWriteFast(LED_BUILTIN,HIGH);
}
ActivityMeasurer::~ActivityMeasurer(){
    digitalWriteFast(LED_BUILTIN,LOW);
}
void ActivityMeasurer::kill(){
    killed = true;
}
bool ActivityMeasurer::isKilled(){
    return killed;
}

ActivityAndExecutionTimeMeasurer::ActivityAndExecutionTimeMeasurer(const char* name) : killed(false){
    this->name = name;
    digitalWriteFast(LED_BUILTIN,HIGH);
    startTime = micros();
}
ActivityAndExecutionTimeMeasurer::~ActivityAndExecutionTimeMeasurer(){
    uint32_t end = micros();
    Serial.printf("%s took %i us\n",name,end-startTime);
    digitalWriteFast(LED_BUILTIN,LOW);
}
void ActivityAndExecutionTimeMeasurer::kill(){
    killed = true;
}
bool ActivityAndExecutionTimeMeasurer::isKilled(){
    return killed;
}