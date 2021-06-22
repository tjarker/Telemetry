/*******************************************************************************
 * @file    Measure.h                                                          *
 * @author  Tjark Petersen                                                     *
 * @brief   This file contains a code environment MEASURE which keeps track of *
 *          the execution time of the code block and reports it via Serial.    *
 *******************************************************************************/ 

#ifndef __MEASURE_H__
#define __MEASURE_H__
#include <Arduino.h>
#include <ChRt.h>

/**
 * @brief   
 * @param   name ?
 * @param   t0 ?
 * @return  void
*/ 
void measureEpilogue(const char *name, uint32_t t0){
    char str[64];
    snprintf(str,64,"%s took %lu us\n",name,micros()-t0);
    Serial.print(str);
}

/**
 * @brief Run measureEpilogue() and print out the execution time with the label "name". 
*/
#define MEASURE(name) for (uint32_t t0 = micros(), i = 0; i < 1; i++, measureEpilogue(name, t0))

#endif