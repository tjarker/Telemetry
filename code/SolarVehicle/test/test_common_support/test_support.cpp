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
#include <sys/stat.h>
#include <ctime>
#include <fstream>
#include <ChRt.h>
#include <ACAN.h>

#include "TelemetryMessages.h"
#include "Encryption.h"
#include "RFfunctions.h"
#include "MultiReaderFifo.h"
#include "ThreadState.h"

CANMessage frame;
CanTelemetryMsg CANmsg;
Security security;

void test_sanity(void) {
    TEST_ASSERT_EQUAL(32, 25 + 7);
    TEST_ASSERT_EQUAL(20, 23 - 3);
    TEST_ASSERT_EQUAL(50, 25 * 2);
    TEST_ASSERT_EQUAL(32, 96 / 3);
}

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
    uint8_t msg[16], msgcon[16];
    uint16_t array[16];
    int cnten = 0;
    security.encryption_key();
    // Random test
    for(int j = 0; j < 100; j++){
        for(int i = 0; i < 16; i++){
            msg[i] = random(0,256);
            msgcon[i] = msg[i];
        }
        security.encrypt(msg, array, 16);
        for (int i = 0; i < 16; i++){
            if (msg[i] != msgcon[i]){
                cnten++;
            }
        }
        // Takes into account that a data value might not be changed after encryption process.
        // The integer used for comparison is the desired minimum amount of changed values.
        TEST_ASSERT_TRUE(cnten > 13);
        security.decrypt(array, msg, 16);
        for(int i = 0; i < 16; i++){
            TEST_ASSERT_TRUE(msg[i] == msgcon[i]);
        }
    }
    // Edge case test
    uint8_t msg2[16];
    memset(msg2,0,sizeof(msg2));
    uint16_t array2[16];
    uint8_t msgcon2[16];
    memset(msgcon2,0,sizeof(msg2));
    security.encrypt(msg2, array2, 16);
    for (int i = 0; i < 16; i++){
        if (msg[i] != msgcon[i]){
            cnten++;
        }
    }
    // Takes into account that a data value might not be changed after encryption process.
    // The integer used for comparison is the desired minimum amount of changed values.
    TEST_ASSERT_TRUE(cnten > 13);
    security.decrypt(array, msg, 16);
    for(int i = 0; i < 16; i++){
        TEST_ASSERT_TRUE(msg[i] == msgcon[i]);
    }

    uint8_t msg3[16];
    memset(msg3,255,sizeof(msg3));
    uint16_t array3[16];
    uint8_t msgcon3[16];
    memset(msgcon3,255,sizeof(msgcon3));
    security.encrypt(msg3, array3, 16);
    for (int i = 0; i < 16; i++){
        if (msg[i] != msgcon[i]){
            cnten++;
        }
    }
    // Takes into account that a data value might not be changed after encryption process.
    // The integer used for comparison is the desired minimum amount of changed values.
    TEST_ASSERT_TRUE(cnten > 13);
    security.decrypt(array3, msg3, 16);
    for(int i = 0; i < 16; i++){
        TEST_ASSERT_TRUE(msg[i] == msgcon[i]);
    }
}

void test_fail(void) {
    TEST_ASSERT_TRUE(false);
}

void process() {
    UNITY_BEGIN();
    RUN_TEST(test_sanity);
    RUN_TEST(test_encryption_key);
    RUN_TEST(test_encrypt_decrypt);
    RUN_TEST(test_fail);
    UNITY_END();
}

#ifdef ARDUINO

#include <Arduino.h>
void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    process();
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
