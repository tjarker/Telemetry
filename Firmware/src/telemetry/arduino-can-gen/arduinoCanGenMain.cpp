#include <Arduino.h>

#include <SPI.h>
#include <mcp2515.h>
#include <ChRt.h>
#include "telemetry/arduino-can-gen/ansi.h"


MUTEX_DECL(printMutex);
MUTEX_DECL(mcpMutex);

struct can_frame canMsg;
MCP2515 mcp2515(10);

bool sending = true, listening = true;

THD_WORKING_AREA(waCanSenderThd,256);
THD_FUNCTION(canSenderThd, arg){

    struct can_frame canMsg;
    uint32_t msgCount = 0;

    randomSeed(analogRead(0));

    while(true){

        if(sending){
            canMsg.can_id  = random(0,0x800);
            canMsg.can_dlc = 8;//random(1,9)
            /*for(int i = 0; i < canMsg.can_dlc; i++){
                canMsg.data[i] = random(0,0x100);
            }*/
            *((uint64_t*)canMsg.data) = msgCount;
            
            chMtxLock(&mcpMutex);
            mcp2515.sendMessage(&canMsg);
            chMtxUnlock(&mcpMutex);

            msgCount++;

            if(Serial){
                chMtxLock(&printMutex);
                printGoto(26,3);
                printClearLine();
                Serial.print(canMsg.can_id,HEX);
                Serial.print(" (");
                Serial.print(canMsg.can_dlc);
                Serial.print(") : ");
                for(int i = canMsg.can_dlc-1; i >= 0; i--){
                    Serial.print(canMsg.data[i],HEX);
                    Serial.print(" ");
                }
                Serial.println();
                printGoto(26,4);
                printClearLine();
                Serial.print(msgCount);
                printGoto(0,9);
                chMtxUnlock(&printMutex);
            }
        }
        
        chThdSleepMicroseconds(random(4000,50000));
    }

}

THD_WORKING_AREA(waCanListener,256);
THD_FUNCTION(canListenerThd, arg){

    struct can_frame canMsg;
    uint32_t msgCount = 0;

    while(true){
        if(listening){
            if(mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK){

                if(Serial){
                    chMtxLock(&printMutex);
                    printGoto(26,6);
                    printClearLine();
                    Serial.print(canMsg.can_id,HEX);
                    Serial.print(" (");
                    Serial.print(canMsg.can_dlc);
                    Serial.print(") : ");
                    for(int i = canMsg.can_dlc-1; i >= 0; i--){
                        Serial.print(canMsg.data[i],HEX);
                        Serial.print(" ");
                    }
                    Serial.println();
                    printGoto(26,7);
                    printClearLine();
                    Serial.print(msgCount);
                    printGoto(0,9);
                    chMtxUnlock(&printMutex);

                }

                msgCount++;

            }
        }
        
        chThdSleepMilliseconds(100);
    }
}

thread_t *senderThd;
thread_t *listenerThd;

void chSetup(){
    chSysInit();
    senderThd = chThdCreateStatic(waCanSenderThd, sizeof(waCanSenderThd), NORMALPRIO+1, canSenderThd, NULL);
    listenerThd = chThdCreateStatic(waCanListener, sizeof(waCanListener), NORMALPRIO+1, canListenerThd, NULL);
}



void setup(){

    SPI.begin();
    
    mcp2515.reset();
    mcp2515.setBitrate(CAN_125KBPS,MCP_8MHZ);
    mcp2515.setNormalMode();

    chBegin(chSetup);
}


void loop() {
    while (!Serial);
    Serial.begin(115200);

    clearScreen();

    printGoto(0,3);
    Serial.println("Last sent message      | -");
    Serial.println("Sent messages          | 0");
    Serial.println();
    Serial.println("Last received message  | -");
    Serial.println("Received messages      | 0");

    while(true){
        if(Serial.available()){
            uint8_t code = Serial.read();

            if(code == 's'){
                sending = !sending;
            } else if(code == 'r'){
                listening = !listening;
            }

        }
        chThdSleepMilliseconds(100);
    }
    
}
