#include<Arduino.h>

#include "TelemetryMessages.h"


CanTelemetryMsg msg;

void sendRandomMessage(){
    char str[128];
    msg.randomize();
    msg.toJSON(str,sizeof(str));
    Serial.print(str);
}

int main(){
    pinMode(LED_BUILTIN,OUTPUT);
    Serial.begin(921600);
    while(!Serial){}

    randomSeed(analogRead(0));

    while(true){
        sendRandomMessage();
        digitalWriteFast(LED_BUILTIN,HIGH);
        delay(100);
        digitalWriteFast(LED_BUILTIN,LOW);
        delay(900);
    }

    return 0;
}