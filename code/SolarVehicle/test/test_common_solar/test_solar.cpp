
#include <unity.h>
#include <ACAN.h>
#include <sys/stat.h>
#include <ctime>
#include <fstream>
#include <Arduino.h>

#include <ACAN.h>

#include <TelemetryMessages.h>
#include <Encryption.h>

void test_sanity(void) {
    TEST_ASSERT_EQUAL(32, 25 + 7);
    TEST_ASSERT_EQUAL(20, 23 - 3);
    TEST_ASSERT_EQUAL(50, 25 * 2);
    TEST_ASSERT_EQUAL(32, 96 / 3);
}

void test_fail(void) {
    TEST_ASSERT_TRUE(false);
}

// --------------------------------- setup --------------------------------- //

// Should test that both CAN0 port and CAN1 port is active
void test_can_x_begin(void) {
    ACANSettings settings (125 * 1000); // Sets wished bitrate
    TEST_ASSERT_TRUE(!ACAN::can0.begin (settings));
    TEST_ASSERT_TRUE(!ACAN::can1.begin (settings));
}

// -------------------------- TelemetryMessages.h -------------------------- //

// Should a string with CAN data in the format hour, minutes, seconds, id, rtr, len data
// Due to lack of support, only base 16 (hexadecimal) is tested and expected to pass, other tests will fail
//uint32_t toString(char *buf, uint32_t len, uint8_t base)
void test_toString(void){
    CanTelemetryMsg CANmsg;
    CANmsg.randomize();
    char testbuf[128];
    char shouldBe[128];
    // Hexadecimal test
    snprintf(shouldBe,sizeof(shouldBe),"\"%02d/%02d/%4d %02d-%02d-%02d\",%X,%X,%X,%llX", day(), month(), year(), CANmsg.h, CANmsg.m, CANmsg.s, CANmsg.id,
        CANmsg.rtr, CANmsg.len, CANmsg.data64);
    CANmsg.toString(testbuf, sizeof(testbuf), 16);
    TEST_ASSERT_EQUAL_STRING(shouldBe, testbuf);
    // Decimal test
    snprintf(shouldBe,sizeof(shouldBe),"\"%02d/%02d/%4d %02d-%02d-%02d\",%D,%D,%D,%llD", day(), month(), year(), CANmsg.h, CANmsg.m, CANmsg.s, CANmsg.id,
        CANmsg.rtr, CANmsg.len, CANmsg.data64);
    CANmsg.toString(testbuf, sizeof(testbuf), 10);
    TEST_ASSERT_EQUAL_STRING(shouldBe, testbuf);
    // Octal
    snprintf(shouldBe,sizeof(shouldBe),"\"%02d/%02d/%4d %02d-%02d-%02d\",%0,%0,%0,%ll0", day(), month(), year(), CANmsg.h, CANmsg.m, CANmsg.s, CANmsg.id,
        CANmsg.rtr, CANmsg.len, CANmsg.data64);
    CANmsg.toString(testbuf, sizeof(testbuf), 8);
    TEST_ASSERT_EQUAL_STRING_MESSAGE(shouldBe, testbuf, "Not yet implemented");
    // Binary
    snprintf(shouldBe,sizeof(shouldBe),"\"%02d/%02d/%4d %02d-%02d-%02d\",%B,%B,%B,%llB", day(), month(), year(), CANmsg.h, CANmsg.m, CANmsg.s, CANmsg.id,
        CANmsg.rtr, CANmsg.len, CANmsg.data64);
    CANmsg.toString(testbuf, sizeof(testbuf), 2);
    TEST_ASSERT_EQUAL_STRING_MESSAGE(shouldBe, testbuf, "Not yet implemented");
}

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

//------------------------------ Encryption.h ----------------------------- //

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

//TODO: fix with new encryption stuff
// Should test that an input message is encrypted
//void encrypt(uint16_t *message, int len)
//void decrypt(uint16_t *message, int len)
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

void process() {
    UNITY_BEGIN();
    RUN_TEST(test_sanity);
    //RUN_TEST(test_fail);
    RUN_TEST(test_can_x_begin);
    //RUN_TEST(test_toString);
    RUN_TEST(test_toString2);
    RUN_TEST(test_getHeader);
    RUN_TEST(test_toCanFrame);
    RUN_TEST(test_update);
    RUN_TEST(test_encryption_key);
    RUN_TEST(test_encrypt_decrypt);
    UNITY_END();
}

void setup() {

    delay(2000);
    
    Serial.begin(921600);
    while(!Serial){}

    Serial.println("Hello world!!!!!!!!");

    process();
}

void loop() {
    delay(100);
}
    