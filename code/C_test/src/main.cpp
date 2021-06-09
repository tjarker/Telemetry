//#include <Arduino.h>

/*#include "avr8-stub.h"
#include "app_api.h" // only needed with flash breakpoints*/
#include "Encryption.h"
#include <stdio.h>
#include <string.h>

Security sc;
BaseTelemetryMsg msg;

int main(){
    sc.encryption_key();

    while(1){
        msg = {(cmd_t) 2, 5, 4, 12, 54, 85, 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
        sc.encrypt((uint8_t*) &(msg), 32);
        sc.decrypt(&msg, 32);
    }
}