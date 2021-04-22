#include <Arduino.h>

#include <SPI.h>
#include <mcp2515.h>
#include <ChRt.h>


MUTEX_DECL(printMutex);
MUTEX_DECL(mcpMutex);

struct can_frame globalCanMsg;
MCP2515 mcp2515(10);

bool sending = true, listening = true;

THD_WORKING_AREA(waCanSenderThd,256);
THD_FUNCTION(canSenderThd, arg){

    struct can_frame canMsg;
    uint32_t msgCount = 0;

    randomSeed(analogRead(0));

    while(true){

        if(sending){
            canMsg.can_id  = random(0,0x1000);
            canMsg.can_dlc = random(1,9);
            for(int i = 0; i < canMsg.can_dlc; i++){
                canMsg.data[i] = random(0,0x100);
            }
            
            chMtxLock(&mcpMutex);
            mcp2515.sendMessage(&canMsg);
            chMtxUnlock(&mcpMutex);

            msgCount++;

            if(Serial){
                chMtxLock(&printMutex);
                Serial.print("<s>{\"id\":");
                Serial.print(canMsg.can_id);
                Serial.print(",\"len\":");
                Serial.print(canMsg.can_dlc);
                Serial.print(",\"data\":");
                for(int i = canMsg.can_dlc-1; i >= 0; i--){
                    Serial.print(canMsg.data[i]);
                }
                Serial.print("}<e>");
                chMtxUnlock(&printMutex);
            }
        }

        for(uint8_t i = 0; i < 8; i++){
            canMsg.data[i] = 0;
        }
        
        chThdSleepMilliseconds(random(50,1500));
    }

}

THD_WORKING_AREA(waSerialListener, 256);
THD_FUNCTION(serialListenerThd, arg){

    

    while(true){
        if(Serial.available()){
            chMtxLock(&printMutex);

            uint8_t numBytes = Serial.readBytes((uint8_t*)&globalCanMsg, 13);

            char outBuf[256];
            uint64_t *tmp = (uint64_t*) (&globalCanMsg + 5);
            snprintf(outBuf,256,"<s>{\"id\":%d,\"len\":%d,\"data\":%llu}<e>",(uint16_t)globalCanMsg.can_id,globalCanMsg.can_dlc,*tmp);

            Serial.print(outBuf);

            chMtxUnlock(&printMutex);

        }
        chThdSleepMicroseconds(200);
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
    //senderThd = chThdCreateStatic(waCanSenderThd, sizeof(waCanSenderThd), NORMALPRIO+1, canSenderThd, NULL);
    chThdCreateStatic(waSerialListener, sizeof(waSerialListener), NORMALPRIO + 1, serialListenerThd, NULL);
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
