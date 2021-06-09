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
        //sendRandomMessage();
        if(Serial.available()){
            Serial.readBytes((char*)&msg,32);
            msg.cmd = 0;
            char str[256];
            msg.toJSON(str,256);
            Serial.println(str);
            digitalWriteFast(LED_BUILTIN,HIGH);
            delay(100);
            digitalWriteFast(LED_BUILTIN,LOW);
            delay(900);
        }
    }

    return 0;
}