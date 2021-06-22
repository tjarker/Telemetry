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
class CANMessage {
  public : uint32_t id = 0 ;  // Frame identifier
  public : bool ext = false ; // false -> standard frame, true -> extended frame
  public : bool rtr = false ; // false -> data frame, true -> remote frame
  public : uint8_t idx = 0 ;  // This field is used by the driver
  public : uint8_t len = 0 ;  // Length of data (0 ... 8)
  public : union {
    uint64_t data64        ; // Caution: subject to endianness
    uint32_t data32 [2]    ; // Caution: subject to endianness
    uint16_t data16 [4]    ; // Caution: subject to endianness
    float    dataFloat [2] ; // Caution: subject to endianness
    uint8_t  data   [8] = {0, 0, 0, 0, 0, 0, 0, 0} ;
  } ;
} ;
CANMessage frame;
CanTelemetryMsg CANmsg;
Security security;
ThreadState thrd;
Fifo<CanTelemetryMsg> testFifo(4);

void test_sanity(void) {
    TEST_ASSERT_EQUAL(32, 25 + 7);
    TEST_ASSERT_EQUAL(20, 23 - 3);
    TEST_ASSERT_EQUAL(50, 25 * 2);
    TEST_ASSERT_EQUAL(32, 96 / 3);
}
/*
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
// uint32_t toString(char *buf, uint32_t len, uint8_t base)
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
    char *testbuf;
    // Hexadecimal test
    for (int i = 0; i < 100; i++){
        CANmsg.randomize();
        CANmsg.toString(testbuf, CANmsg.len);
        TEST_ASSERT_EQUAL_STRING(("\"%02d-%02d-%02d\",%u,%u,%u,%llu", CANmsg.h, CANmsg.m, CANmsg.s, CANmsg.id,
            CANmsg.rtr, CANmsg.len, CANmsg.data64), testbuf);
    }
}

// Should test that a string header is returned in the format \"time\",\"id\",\"rtr\",\"len\",\"data\"
//static const String getHeader()
void test_getHeader(void){
    //TEST_ASSERT_EQUAL_STRING(CANmsg.getHeader(), const String("\"time\",\"id\",\"rtr\",\"len\",\"data\""));
}

// Should set data in CAN frame
//void toCanFrame(CANMessage *msg)
void test_toCanFrame(void){
    CANMessage *testmsg;
    for (int i = 0; i < 100; i++){
        testmsg->id = random(0, 256);
        testmsg->rtr = rand()%2;
        testmsg->len = 8;
        testmsg->data64 = random(0, 0xFFFFFFFFFFFFFFFF);
        CANmsg.toCanFrame(testmsg);
        TEST_ASSERT_EQUAL_INT(testmsg->id, CANmsg.id);
        TEST_ASSERT_EQUAL_INT(testmsg->rtr, CANmsg.rtr);
        TEST_ASSERT_EQUAL_INT(testmsg->len, CANmsg.len);
        TEST_ASSERT_EQUAL_INT(testmsg->data64, CANmsg.data64);
    }
}

// Should update time stamp on CAN message
// void update(CANMessage *msg)
void test_update(void){
    CANMessage MsgTest;
    CANmsg.randomize();
        /*char tmp[200]; // Debug lines
    snprintf(tmp,200,"%d,%d,%d,%llu",test_msg.id,test_msg.rtr,test_msg.len,test_msg.data64);
    Serial.println(tmp);
    CANmsg.update(&MsgTest);
    char test_str[200];
    /*snprintf(test_str,200,"\"%02d/%02d/%04d %02d-%02d-%02d\",%" PRIx16 ",%d,%d,%" PRIx64 "", 
        day(), month(), year(), hour(), minute(), second(), test_msg.id, test_msg.rtr, test_msg.len, 
        test_msg.data64);
    TEST_ASSERT_EQUAL_STRING(test_str, CANMessage.toString());
}

// Should set correct time in seconds, minutes and hours
//void stamp()

// ----------------------------- ThreadState.h ----------------------------- //

// Should test that a thread can be suspended
// void suspend()
void test_suspend(void){
    // Hvordan testes dette
}

// Should test that a thread is no longer paused
// void wakeUp()
void test_wakeUp(void){
    thrd.pause = true;
    thrd.wakeUp();
    TEST_ASSERT_FALSE(thrd.pause);
}

// --------------------------------- Fifo.h -------------------------------- //

// 
// void advance(uint32_t *index)
void test_advance(void){
    uint32_t *index;
    index = 0;
    testFifo.advance(index);
    TEST_ASSERT_EQUAL_INT(index, 1);
}

//
// uint32_t getSize()
void test_getSize(void){
    TEST_ASSERT_EQUAL_INT32(testFifo.getSize(), 4);
}

// Should test that the fifos data is set to 0 at all indexes
// void clear()
void test_clear(void){
    testFifo.clear();
    for (int i = 0; i < sizeof(testFifo); i++){
        /*uint8_t j = testFifo;
        TEST_ASSERT_EQUAL_INT(j, 0);
        testFifo.advance(i);
    }
}

// Should test that the fifo is full
// bool full()
void test_full(void){
    //TEST_ASSERT_FALSE(testFifo.full); a pointer to a bound function may only be used to call the function
}

// Should test that the fifo is empty           Skal gøres bedre
// bool empty()
void test_empty(void){
    TEST_ASSERT_TRUE(testFifo.empty());

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
    uint16_t array2[16];
    uint8_t msgcon2[16];
    memset(msg2, 0, sizeof(msg2)); 
    memset(msgcon2, 0, sizeof(msgcon2)); 
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
    uint16_t array3[16];
    uint8_t msgcon3[16]; 
    memset(msg3, 255, sizeof(msg3)); 
    memset(msgcon3, 255, sizeof(msgcon3)); 
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
*/
void test_fail(void) {
    TEST_ASSERT_TRUE(false);
}

void process() {
    UNITY_BEGIN();
    RUN_TEST(test_sanity);
    /*RUN_TEST(test_RFinit);
    RUN_TEST(test_RFtransmit);
    RUN_TEST(test_RFreceive);
    RUN_TEST(test_encryption_key);
    RUN_TEST(test_encrypt_decrypt);*/
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
