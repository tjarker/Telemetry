
#include <unity.h>
#include <Arduino.h>

#include <TelemetryMessages.h>
#include <Encryption.h>
#include <RFfunctions.h>
#include <BlackBox.h>

void test_sanity(void) {
    TEST_ASSERT_EQUAL(32, 25 + 7);
    TEST_ASSERT_EQUAL(20, 23 - 3);
    TEST_ASSERT_EQUAL(50, 25 * 2);
    TEST_ASSERT_EQUAL(32, 96 / 3);
}

void test_fail(void) {
    TEST_ASSERT_TRUE(false);
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
    snprintf(shouldBe,sizeof(shouldBe),"\"%02d-%02d-%02d\",%X,%X,%X,%llX", CANmsg.h, CANmsg.m, CANmsg.s, CANmsg.id,
        CANmsg.rtr, CANmsg.len, CANmsg.data64);
    CANmsg.toString(testbuf, sizeof(testbuf), 16);
    TEST_ASSERT_EQUAL_STRING(shouldBe, testbuf);
    // Decimal test
    snprintf(shouldBe,sizeof(shouldBe),"\"%02d-%02d-%02d\",%d,%d,%d,%lld", CANmsg.h, CANmsg.m, CANmsg.s, CANmsg.id,
        CANmsg.rtr, CANmsg.len, CANmsg.data64);
    CANmsg.toString(testbuf, sizeof(testbuf), 10);
    TEST_ASSERT_EQUAL_STRING(shouldBe, testbuf);
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

// ----------------------------- RFfunctions.h ----------------------------- //

// Should test that RF is initialized
// bool RFinit()
void test_RFinit(void){
    TEST_ASSERT_TRUE(RFinit());
}

void process() {
    UNITY_BEGIN();
    RUN_TEST(test_sanity);
    RUN_TEST(test_toString);
    RUN_TEST(test_toString2);
    RUN_TEST(test_encryption_key);
    RUN_TEST(test_encrypt_decrypt);
    RUN_TEST(test_RFinit);
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
    