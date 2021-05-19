#ifndef __MEASURE_H__
#define __MEASURE_H__

#include <Arduino.h>
#include <ChRt.h>

/**
 * This file contains a code environment MEASURE which keeps track of the execution time of
 * the code block and reports it via Serial
 */

class ExecutionTimeMeasurer{
    private:
        bool killed = false;
        const char* name;
        uint32_t startTime;
    public:
        ExecutionTimeMeasurer(const char* name) {
            this->name = name;
            startTime = micros();
        }
        ~ExecutionTimeMeasurer(){
            Serial.printf("%s took %i us\n",name,micros()-startTime);
        }
        void kill(){killed = true;}
        bool isKilled(){return killed;}
};

/**
 * Execute the provided code block and print out the execution time with the label "name"
 */
#define MEASURE(name) for (ExecutionTimeMeasurer x(name); !x.isKilled(); x.kill())

#endif