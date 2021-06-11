/*
 Copyright (c) 2014-present PlatformIO <contact@platformio.org>

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
**/

// Maybe use pio --without-building?

#include <unity.h>
#include <ACAN.h>
#include <sys/stat.h>
#include <ctime>
#include <fstream>
#include <ChRt.h>

#include "BlackBox.h"
#include "TelemetryMessages.h"
#include "Encryption.h"
#include "RFfunctions.h"
#include "MultiReaderFifo.h"
#include "ThreadState.h"
#include "solar-car/blackBoxThd.h"
#include "solar-car/CanReceiverThd.h"
#include "solar-car/Mutexes.h"
#include "solar-car/RfTxThd.h"
#include "solar-car/SystemThd.h"

CANMessage frame;
BlackBox box;
Security security;
MultiReaderFifo<CanTelemetryMsg> canFifo(64,2);
ThreadState blackBoxWorkerState;
ThreadState canReceiverState;
ThreadState rfWorkerState;

// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

void test_sanity(void) {
    TEST_ASSERT_EQUAL(32, 25 + 7);
    TEST_ASSERT_EQUAL(20, 23 - 3);
    TEST_ASSERT_EQUAL(50, 25 * 2);
    TEST_ASSERT_EQUAL(32, 96 / 3);
}

// --------------------------------- setup --------------------------------- //

// Should test that both CAN0 port and CAN1 port is active
void test_can_x_begin(void) {
    ACANSettings settings (125 * 1000); // Sets wished bitrate
    const uint32_t can0_begin = ACAN::can0.begin (settings);
    const uint32_t can1_begin = ACAN::can1.begin (settings);
    TEST_ASSERT_TRUE(can0_begin);
    TEST_ASSERT_TRUE(can1_begin);
}

// -------------------------- TelemetryMessages.h -------------------------- //

//uint32_t toString(char *buf, uint32_t len, uint8_t base)

//uint32_t toString(char *buf, uint32_t len)

//static const String getHeader()

// Should set data in CAN frame
//void toCanFrame(CANMessage *msg)

// Should update time stamp on CAN message
//void update(CANMessage *msg)

// Should set correct time in seconds, minutes and hours
//void stamp()

// ------------------------------ Encryption.h ----------------------------- //

// Should test that an encryption array from Security class is made
// Test does not take into account that an encryption key could be all zero
// void encryption_key()
void test_encryption_key(void){
    int cnte, cntd = 0;
    for (int i = 0; i < 256; i++){
        if (security.e[i] != 0){
            cnte++;
        }
        if (security.d[i] != 0){
            cntd++;
        }
    }
    TEST_ASSERT_TRUE(cnte);
    TEST_ASSERT_TRUE(cntd);
}

// Should test that an input message is encrypted
//void encrypt(uint16_t *message, int len)
//void decrypt(uint16_t *message, int len)
void test_encrypt_decrypt(void){
    uint16_t msg[16], msgcon[16];
    int cnten = 0;
    security.encryption_key();
    // Random test
    for(int j = 0; j < 100; j++){
        for(int i = 0; i < 16; i++){
            msg[i] = random(0,256);
            msgcon[i] = msg[i];
        }
        security.encrypt(msg, 16);
        for (int i = 0; i < 16; i++){
            if (msg[i] != msgcon[i]){
                cnten++;
            }
        }
        // Takes into account that a data value might not be changed after encryption process.
        // The integer used for comparison is the desired minimum amount of changed values.
        TEST_ASSERT_TRUE(cnten > 13);
        security.decrypt(msg, 16);
        for(int i = 0; i < 16; i++){
            TEST_ASSERT_TRUE(msg[i] == msgcon[i]);
        }
    }
    // Edge case test
    uint16_t msg2[16] = {[0 ... 15] = 0};
    uint16_t msgcon2[16] = {[0 ... 15] = 0};
    security.encrypt(msg, 16);
    for (int i = 0; i < 16; i++){
        if (msg[i] != msgcon[i]){
            cnten++;
        }
    }
    // Takes into account that a data value might not be changed after encryption process.
    // The integer used for comparison is the desired minimum amount of changed values.
    TEST_ASSERT_TRUE(cnten > 13);
    security.decrypt(msg, 16);
    for(int i = 0; i < 16; i++){
        TEST_ASSERT_TRUE(msg[i] == msgcon[i]);
    }

    uint16_t msg3[16] = {[0 ... 15] = 255};
    uint16_t msgcon3[16] = {[0 ... 15] = 255};
    security.encrypt(msg, 16);
    for (int i = 0; i < 16; i++){
        if (msg[i] != msgcon[i]){
            cnten++;
        }
    }
    // Takes into account that a data value might not be changed after encryption process.
    // The integer used for comparison is the desired minimum amount of changed values.
    TEST_ASSERT_TRUE(cnten > 13);
    security.decrypt(msg, 16);
    for(int i = 0; i < 16; i++){
        TEST_ASSERT_TRUE(msg[i] == msgcon[i]);
    }
}

// ----------------------------- RFfunctions.h ----------------------------- //

// Should test that the RF module is initialized
// void RFinit()

// Should test that the RF module is transmitting
// bool RFtransmit(void *buf, uint8_t len)

// Should test that the RF module is receiving
//bool RFreceive(void *buf, uint8_t len)

// ------------------------------- BlackBox.h ------------------------------ //

// Should test that the blackbox initializes
void test_init(void){
    bool t = box.init();
    TEST_ASSERT_TRUE(t);
}

// Should test that the Teensy 3.6 real time clock is correct within 1 second
void test_getTeensy3Time(void){
    char test_str[200];
    snprintf(test_str,200,"\"%02d/%02d/%04d %02d-%02d-%02d\",%" PRIx16 ",%d,%d,%" PRIx64 "", 
    day(), month(), year(), hour(), minute(), second(), frame.id, frame.rtr, frame.len, 
    frame.data64);
    // Forkerte typer, men basale princip er nedenstående
    TEST_ASSERT_INT_WITHIN(1, test_str, box.getTeensy3Time);
}

// Should test that a new log file is started
void test_startNewLogFile(void){
    box.startNewLogFile();
    struct stat buffer;
    TEST_ASSERT_TRUE((stat ("Tjark, hvor ligger filen?", &buffer) == 0));
}

// Should test that the current log file is ended
void test_endLogFile(void){
    box.endLogFile();
    //TEST_ASSERT_TRUE(box.sdBuffer.bytesFree()); // bytesFree should return 0
}

// Should test that a new line is added in .csv log
void test_addNewLogStr(void){
    /*int i = 0;
    fstream file;
    file.open(("log__%02d_%02d_%02d__%02d_%02d_%04d.csv", hour(), minute(), second(),
    day(), month(), year()), ios::in);
    vector<string> row;
    string line, word, temp;
    while(file >> temp){
        row.clear();
        getline(file, line);
        stringstream s(line);
        while (getline(s, word, ', ')) {
            row.push_back(word);
        }
        while (row[i].empty()){
            i++;
        }
        // Hvordan kan jeg sikre mig at det er en ny linje, og at den ikke overskriver den eksisterende linje?
    }*/
}

// ------------------------------- Fail test ------------------------------- //

// Should test that a test can fail
void test_fail(void) {
    TEST_ASSERT_TRUE(false);
}

void process() {
    UNITY_BEGIN();
    RUN_TEST(test_sanity);
    RUN_TEST(test_can_x_begin);
    RUN_TEST(test_init);
    RUN_TEST(test_fail);
    RUN_TEST(test_encryption_key);
    RUN_TEST(test_encrypt_decrypt);
    RUN_TEST(test_getTeensy3Time);
    RUN_TEST(test_startNewLogFile);
    RUN_TEST(test_endLogFile);
    UNITY_END();
}

#ifdef ARDUINO

void chSetup(){

  chSysInit();
  
  // create the three worker threads
  BlackboxWorkerBundle blackBoxWorkerBundle = {.fifo = &canFifo, .state = &blackBoxWorkerState, .bb = &box};
  chThdCreateStatic(blackBoxWorker, sizeof(blackBoxWorker), NORMALPRIO + 3, canWorkerFunc, &blackBoxWorkerBundle);
  rfTxWorkerBundle rfWorkerBundle = {.fifo = &canFifo, .state = &rfWorkerState, .sec = &security};
  chThdCreateStatic(waRfWorker,sizeof(waRfWorker),NORMALPRIO + 2, rfWorker, &rfWorkerBundle);
  systemThdBundle systemThdBundle = {.canReceiverState = &canReceiverState, .blackBoxWorkerState = &blackBoxWorkerState, .rfWorkerState = &rfWorkerState, .sec = &security};
  chThdCreateStatic(waSystemThd, sizeof(waSystemThd), NORMALPRIO + 1, systemThd, &systemThdBundle);
  CanReceiverBundle canReceiverBundle = {.fifo = &canFifo, .state = &canReceiverState};
  chThdCreateStatic(waCanReceiver, sizeof(waCanReceiver), NORMALPRIO + 1, canReceiverThd, &canReceiverBundle);
  
}

#include <Arduino.h>
void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
      // initialize serial port
    
    Serial.begin(921600);
    while(!Serial){} //needs to be removed when headless!!!!!!!!!!!!!!!!!!!!

    // setup built in LED
    pinMode(LED_BUILTIN,OUTPUT);

    Serial.println("Initializing...");

    // setup CAN bus
    ACANSettings settings(125 * 1000);
    if(ACAN::can0.begin(settings) != 0){Serial.println("CAN setup failed!");}

    // setup radio module
    RFinit();

    box.init();

    security.encryption_key();

    canFifo.clear();

    Serial.println("Starting...");

    chBegin(chSetup);
}

void loop() {
    //digitalWrite(13, HIGH);
    delay(100);
    //digitalWrite(13, LOW);
    delay(500);
}

#else

int main(int argc, char **argv) {
    process();
    return 0;
}

#endif
