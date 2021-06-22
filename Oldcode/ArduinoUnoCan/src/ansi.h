#ifndef __ANSI_H__
#define __ANSI_H__

#include <Arduino.h>

void printGoto(uint8_t x, uint8_t y) {
    char str[8];
    snprintf(str,8,"\033[%d;%dH",y,x);
    Serial.print(str);
}

void printClearLine() {
    Serial.print("\033[K");
}

void clearScreen(){
    Serial.println("\033[2J");
}

#endif