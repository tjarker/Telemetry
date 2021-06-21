/****************************************************************************************************
 * @file    test_support.cpp                                                                        *
 * @author  Victor Alexander Hansen                                                                 *
 * @brief   Test file for testing various functions made for the telemetry module for the ROAST     *
 *      support car                                                                                 *
 *                                                                                                  *
 * To run the test, open a pio terminal and type "pio test" in PlatformIO                           *
 * Be sure to have the correct board connected for the corresponding environment in the .ini file.  *
 * If the test keeps uploading but nothing happens, try to comment out the other environments in    *
 * the .ini file                                                                                    *
 ****************************************************************************************************/

// Maybe use pio --without-building?

#include <unity.h>
#include <sys/stat.h>
#include <ctime>
#include <fstream>
#include <ChRt.h>

#include "telemetry/support-vehicle/SystemThds.h"

CANMessage frame;
CanTelemetryMsg CANmsg;
Security security;

void test_sanity(void) {
    TEST_ASSERT_EQUAL(32, 25 + 7);
    TEST_ASSERT_EQUAL(20, 23 - 3);
    TEST_ASSERT_EQUAL(50, 25 * 2);
    TEST_ASSERT_EQUAL(32, 96 / 3);
}

// ----------------------------- RFfunctions.h ----------------------------- //

// Should test that RF is initialized
// bool RFinit()
void test_RFinit(void){
    RFinit();
    TEST_ASSERT_TRUE(RFinit());
}

// Should return true when passed a message and false when not passed a message
// bool RFtransmit(void *buf, uint8_t len)
void test_RFtransmit(void){
    RFinit();
    void *truebuf;
    void *falsebuf = '\0';
    TEST_ASSERT_FALSE(RFtransmit(truebuf, 0));
    TEST_ASSERT_FALSE(RFtransmit(falsebuf, 0));
    for (int i = 1; i < 33; i++){
        TEST_ASSERT_TRUE(RFtransmit(truebuf, i));
        TEST_ASSERT_FALSE(RFtransmit(falsebuf, i));
    }
}

// Should
// bool RFreceive(void *buf, uint8_t len)
void test_RFreceive(void){
    RFinit();
    void *truebuf;
    TEST_ASSERT_TRUE(RFreceive(truebuf, 32));
    RFtransmit(truebuf, 32); // Triggers transmitter on pipe
    TEST_ASSERT_FALSE(RFreceive(truebuf, 32));
}

// -------------------------- TelemetryMessages.h -------------------------- //

// Should a string with CAN data in the format hour, minutes, seconds, id, rtr, len data
// Due to lack of support, only base 16 (hexadecimal) is tested and expected to pass, other tests will fail
//uint32_t toString(char *buf, uint32_t len, uint8_t base)
void test_toString(void){
    CANmsg.randomize();
    char *testbuf;
    // Hexadecimal test
    CANmsg.toString(testbuf, CANmsg.len, 16);
    TEST_ASSERT_EQUAL_STRING(("\"%02d-%02d-%02d\",%X,%X,%X,%llX", CANmsg.h, CANmsg.m, CANmsg.s, CANmsg.id,
        CANmsg.rtr, CANmsg.len, CANmsg.data64), testbuf);
    // Decimal test
    CANmsg.toString(testbuf, CANmsg.len, 10);
    TEST_ASSERT_EQUAL_STRING(("\"%02d-%02d-%02d\",%D,%D,%D,%llD", CANmsg.h, CANmsg.m, CANmsg.s, CANmsg.id,
        CANmsg.rtr, CANmsg.len, CANmsg.data64), testbuf);
    // Octal
    CANmsg.toString(testbuf, CANmsg.len, 8);
    TEST_ASSERT_EQUAL_STRING_MESSAGE(("\"%02d-%02d-%02d\",%0,%0,%0,%ll0", CANmsg.h, CANmsg.m, CANmsg.s, CANmsg.id,
        CANmsg.rtr, CANmsg.len, CANmsg.data64), testbuf, "Not yet implemented");
    // Binary
    CANmsg.toString(testbuf, CANmsg.len, 2);
    TEST_ASSERT_EQUAL_STRING_MESSAGE(("\"%02d-%02d-%02d\",%B,%B,%B,%llB", CANmsg.h, CANmsg.m, CANmsg.s, CANmsg.id,
        CANmsg.rtr, CANmsg.len, CANmsg.data64), testbuf, "Not yet implemented");
}


//uint32_t toString(char *buf, uint32_t len)
void test_toString2(void){
    CANmsg.randomize();
    char *testbuf;
    // Hexadecimal test
    CANmsg.toString(testbuf, CANmsg.len);
    TEST_ASSERT_EQUAL_STRING(("\"%02d-%02d-%02d\",%u,%u,%u,%llu", CANmsg.h, CANmsg.m, CANmsg.s, CANmsg.id,
        CANmsg.rtr, CANmsg.len, CANmsg.data64), testbuf);
}

// Should test that a string header is returned in the format \"time\",\"id\",\"rtr\",\"len\",\"data\"
//static const String getHeader()
void test_getHeader(void){
    //TEST_ASSERT_EQUAL_STRING(CANmsg.getHeader(), const String("\"time\",\"id\",\"rtr\",\"len\",\"data\""));
}

// Should set data in CAN frame
//void toCanFrame(CANMessage *msg)

// Should update time stamp on CAN message
// void update(CANMessage *msg)
void test_update(void){
    CANMessage MsgTest;
    CANmsg.randomize();
        /*char tmp[200]; // Debug lines
    snprintf(tmp,200,"%d,%d,%d,%llu",test_msg.id,test_msg.rtr,test_msg.len,test_msg.data64);
    Serial.println(tmp);*/
    CANmsg.update(&MsgTest);
    char test_str[200];
    /*snprintf(test_str,200,"\"%02d/%02d/%04d %02d-%02d-%02d\",%" PRIx16 ",%d,%d,%" PRIx64 "", 
        day(), month(), year(), hour(), minute(), second(), test_msg.id, test_msg.rtr, test_msg.len, 
        test_msg.data64);
    TEST_ASSERT_EQUAL_STRING(test_str, CANMessage.toString());*/
}

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
    uint8_t msg2[16] = {[0 ... 15] = 0};
    uint16_t array2[16];
    uint8_t msgcon2[16] = {[0 ... 15] = 0};
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

    uint8_t msg3[16] = {[0 ... 15] = 255};
    uint16_t array3[16];
    uint8_t msgcon3[16] = {[0 ... 15] = 255};
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
    RUN_TEST(test_RFinit);
    RUN_TEST(test_RFtransmit);
    RUN_TEST(test_RFreceive);
    RUN_TEST(test_encryption_key);
    RUN_TEST(test_encrypt_decrypt);
    RUN_TEST(test_fail);
    UNITY_END();
}

void chSetup()
{
  chSysInit();          // Initializes ChibiOS system
  chThdCreateStatic(WaReceiverThd, sizeof(WaReceiverThd), NORMALPRIO + 1, receiverThd, &sec);
  thdBundle transmitterBundle = {.security = &sec, .fifo = &TXfifo}; 
  chThdCreateStatic(waTransmitterThd, sizeof(waTransmitterThd), NORMALPRIO + 1, transmitterThd, &transmitterBundle);
}

#ifdef ARDUINO

#include <Arduino.h>
void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);
    sec.encryption_key(); // Create message encryption key
    TXfifo.clear();       // Clears TXfifo 
    chBegin(chSetup);     // Initialize and start all 4 threads

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
