#include <Arduino.h>

/*#include "avr8-stub.h"
#include "app_api.h" // only needed with flash breakpoints*/
#include "encryption.h"

Security sc;

void setup(){
    Serial.begin(9600);
    //debug_init();
    while(!Serial){}
    pinMode(LED_BUILTIN,OUTPUT);

    sc.encryption_key();
}

void loop(){
    char *message = "0";
    //Serial.print(String("Debug"));
    //Serial.println(message);
    //Serial.println();
    message = sc.encrypt(message, strlen(message));
    //Serial.println(message);
    //Serial.println();
    message = sc.decrypt(message, strlen(message));
    //Serial.println(message);
    //Serial.println();
    delay(5000);
}