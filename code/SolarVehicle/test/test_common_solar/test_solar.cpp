/*
 Test file for testing various functions made for the telemetry module for the ROAST solar car
 @author Victor Alexander Hansen
 @author Steffan Martin Kunoy
 @author Tjark Petersen

 To run the test, open a pio terminal and type "pio test" in PlatformIO
 Be sure to have the correct board connected for the corresponding environment in the .ini file.
 If the test keeps uploading but nothing happens, try to comment out the other environments in
 the .ini file
**/

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
#include "telemetry/solar-car/blackBoxThd.h"
#include "telemetry/solar-car/CanReceiverThd.h"
#include "telemetry/solar-car/Mutexes.h"
#include "telemetry/solar-car/RfTxThd.h"
#include "telemetry/solar-car/SystemThd.h"

CANMessage frame;
CanTelemetryMsg CANmsg;
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
    RUN_TEST(test_toString);
    RUN_TEST(test_toString2);
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
