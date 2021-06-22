/****************************************************************************************************
 * @file    test_solar.cpp                                                                          *
 * @author  Victor Alexander Hansen                                                                 *
 * @author  Tjark Petersen                                                                          *
 * @author  Steffan Martin Kunoy                                                                    *
 * @brief   Test of various functions in the library depency folder "lib"                           *
 * @note    File must be located in the folder test/test_common_solar                               *
 *                                                                                                  *
 * To run the tests, open a new PlatformIO terminal and type "pio test"                             *
 ****************************************************************************************************/

#include <unity.h>
#include <ACAN.h>
#include <sys/stat.h>
#include <ctime>
#include <fstream>
#include <Arduino.h>

#include <ACAN.h>

#include <TelemetryMessages.h>
#include <Encryption.h>
#include <RFfunctions.h>
#include <BlackBox.h>

BlackBox bb(50);

// Should do a simple sanity test check
void test_sanity(void) {
    TEST_ASSERT_EQUAL(32, 25 + 7);
    TEST_ASSERT_EQUAL(20, 23 - 3);
    TEST_ASSERT_EQUAL(50, 25 * 2);
    TEST_ASSERT_EQUAL(32, 96 / 3);
}

// Should test that a test can fail
void test_fail(void) {
    TEST_ASSERT_TRUE(false);
}

// ------------------------------------------------------------------------- //
// --------------------------------- setup --------------------------------- //
// ------------------------------------------------------------------------- //

// Should test that both CAN0 port and CAN1 port is active
void test_can_x_begin(void) {
    ACANSettings settings (125 * 1000); // Sets wished bitrate
    TEST_ASSERT_TRUE(!ACAN::can0.begin (settings));
    TEST_ASSERT_TRUE(!ACAN::can1.begin (settings));
}

// ------------------------------------------------------------------------- //
// -------------------------- TelemetryMessages.h -------------------------- //
// ------------------------------------------------------------------------- //

// Should test that the content of the CAN message is converted to a string
//uint32_t toString(char *buf, uint32_t len)
void test_toString2(void){
    CanTelemetryMsg CANmsg;
    CANmsg.randomize();
    char testbuf[128];
    // Hexadecimal test
    CANmsg.toString(testbuf, sizeof(testbuf));
    char shouldBe[128];
    snprintf(shouldBe,sizeof(shouldBe),"\"%02d/%02d/%4d %02d-%02d-%02d\",%u,%u,%u,%llu", day(), month(), year(), CANmsg.h, CANmsg.m, CANmsg.s, CANmsg.id,
        CANmsg.rtr, CANmsg.len, CANmsg.data64);
    TEST_ASSERT_EQUAL_STRING(shouldBe, testbuf);
}

// Should test that a string header is returned in the format \"time\",\"id\",\"rtr\",\"len\",\"data\"
//static const String getHeader()
void test_getHeader(void){
    char shouldBe[64] = "\"time\",\"id\",\"rtr\",\"len\",\"data\"", is[64];
    CanTelemetryMsg::getHeader().toCharArray(is,sizeof(is));
    TEST_ASSERT_EQUAL_STRING(shouldBe,is);
}

// Should set data in CAN frame
//void toCanFrame(CANMessage *msg)
void test_toCanFrame(void){
    CanTelemetryMsg CANmsg;
    CANMessage testmsg;
    for (int i = 0; i < 100; i++){
        CANmsg.randomize();
        CANmsg.toCanFrame(&testmsg);
        TEST_ASSERT_EQUAL_INT(testmsg.id, CANmsg.id);
        TEST_ASSERT_EQUAL_INT(testmsg.rtr, CANmsg.rtr);
        TEST_ASSERT_EQUAL_INT(testmsg.len, CANmsg.len);
        TEST_ASSERT_EQUAL_INT(testmsg.data64, CANmsg.data64);
    }
}

// Should update time stamp on CAN message
// void update(CANMessage *msg)
void test_update(void){
    CanTelemetryMsg CANmsg;
    CANMessage testmsg;
    for (int i = 0; i < 100; i++){
        testmsg.id = random(0, 2048);
        testmsg.rtr = rand()%2;
        testmsg.len = 8;
        for(uint32_t i = 0; i < 8; i++) {
            testmsg.data[i] = random(0,0x100);
        }
        CANmsg.update(&testmsg);
        TEST_ASSERT_EQUAL_INT(testmsg.id, CANmsg.id);
        TEST_ASSERT_EQUAL_INT(testmsg.rtr, CANmsg.rtr);
        TEST_ASSERT_EQUAL_INT(testmsg.len, CANmsg.len);
        TEST_ASSERT_EQUAL_INT(testmsg.data64, CANmsg.data64);
        TEST_ASSERT_INT16_WITHIN(1,second(),CANmsg.s);
        TEST_ASSERT_EQUAL(minute(),CANmsg.m);
        TEST_ASSERT_EQUAL(hour(),CANmsg.h);
    }
}

// ------------------------------------------------------------------------- //
//------------------------------ Encryption.h ------------------------------ //
// ------------------------------------------------------------------------- //

// Should test that an encryption array from Security class is made
// Test does not take into account that an encryption key could be all zero
// void encryption_key()
void test_encryption_key(void){
    Security security;
    security.encryption_key();
    int cnte = 0, cntd = 0;
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
// void encrypt(uint16_t *message, int len)
// void decrypt(uint16_t *message, int len)
void test_encrypt_decrypt(void){
    Security security;
    security.encryption_key();
    uint8_t msg[16], msgcon[16];
    uint16_t array[16];
    int cnten = 0;
    // Random test
    for(int j = 0; j < 100; j++){
        for(int i = 0; i < 16; i++){
            msg[i] = random(0,256);
            msgcon[i] = msg[i];
        }
        security.encrypt(msg, array, 16);
        for (int i = 0; i < 16; i++){
            if (array[i] != msgcon[i]){
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
    memset(msgcon2,0,sizeof(msgcon2));
    security.encrypt(msg2, array2, 16);
    cnten = 0;
    for (int i = 0; i < 16; i++){
        if (array2[i] != msgcon2[i]){
            cnten++;
        }
    }
    // Takes into account that a data value might not be changed after encryption process.
    // The integer used for comparison is the desired minimum amount of changed values.
    TEST_ASSERT_TRUE(cnten > 13);
    security.decrypt(array2, msg2, 16);
    for(int i = 0; i < 16; i++){
        TEST_ASSERT_TRUE(msg2[i] == msgcon2[i]);
    }

    uint8_t msg3[16];
    memset(msg3,255,sizeof(msg3));
    uint16_t array3[16];
    uint8_t msgcon3[16];
    memset(msgcon3,255,sizeof(msgcon3));
    security.encrypt(msg3, array3, 16);
    cnten = 0;
    for (int i = 0; i < 16; i++){
        if (array3[i] != msgcon3[i]){
            cnten++;
        }
    }
    // Takes into account that a data value might not be changed after encryption process.
    // The integer used for comparison is the desired minimum amount of changed values.
    TEST_ASSERT_TRUE(cnten > 13);
    security.decrypt(array3, msg3, 16);
    for(int i = 0; i < 16; i++){
        TEST_ASSERT_TRUE(msg3[i] == msgcon3[i]);
    }
}

// ------------------------------------------------------------------------- //
// ----------------------------- RFfunctions.h ----------------------------- //
// ------------------------------------------------------------------------- //

// Should test that RF is initialized
// bool RFinit()
void test_RFinit(void){
    TEST_ASSERT_TRUE(RFinit());
}

// ------------------------------------------------------------------------- //
// ------------------------------- BlackBox.h ------------------------------ //
// ------------------------------------------------------------------------- //

// Should test that the blackbox initializes
// bool init()
void test_init(void){
    bool t = bb.init();
    TEST_ASSERT_TRUE(t);
}

// Should test that a new log file is started
// void startNewLogFile()
void test_startNewLogFile(void){
    bb.startNewLogFile();
    char fileName[64];
    bb.getCurrentFileName(fileName,sizeof(fileName));
    TEST_ASSERT_TRUE(bb.exists(fileName));
}

// Should test that the current log file is ended
// void endLogFile()
void test_endLogFile(void){
    bb.startNewLogFile();
    bb.endLogFile();
    TEST_ASSERT_TRUE(bb.getBytesInBuffer() == 0);
}

// Should test that a new line is added in .csv log
// bool addNewLogStr(CanTelemetryMsg *log)
void test_addNewLogStr(void){
    bb.startNewLogFile();
    CanTelemetryMsg msg;
    msg.randomize();
    bb.addNewLogStr(&msg);
    char shouldBe[128], is[128];
    uint32_t pos = snprintf(shouldBe,sizeof(shouldBe),"\"time\",\"id\",\"rtr\",\"len\",\"data\"\r\n");
    pos += msg.toString(shouldBe+pos,sizeof(shouldBe)-pos);
    snprintf(shouldBe+pos,sizeof(shouldBe)-pos,"\r\n");
    bb.cpyFromBuffer(is,sizeof(is));
    TEST_ASSERT_EQUAL_STRING(shouldBe,is);
}

// Function that calls the tests defined above
void process() {
    UNITY_BEGIN();
    RUN_TEST(test_sanity);
    //RUN_TEST(test_fail);
    RUN_TEST(test_can_x_begin);
    RUN_TEST(test_toString2);
    RUN_TEST(test_getHeader);
    RUN_TEST(test_toCanFrame);
    RUN_TEST(test_update);
    RUN_TEST(test_encryption_key);
    RUN_TEST(test_encrypt_decrypt);
    RUN_TEST(test_RFinit);
    RUN_TEST(test_init);
    RUN_TEST(test_startNewLogFile);
    RUN_TEST(test_endLogFile);
    RUN_TEST(test_addNewLogStr);
    UNITY_END();
}

void setup() {

    delay(2000);
    
    Serial.begin(921600);
    while(!Serial){}

    Serial.println("Executing tests...");

    process();
}

void loop() {
    delay(100);
}
    