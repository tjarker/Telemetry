/* Test for the CAN files
 * Using AUnit library, see the following for more info
 * https://github.com/bxparks/AUnit
 * https://www.arduino.cc/reference/en/libraries/aunit/
 * 
 * Please work ffs
*/

/* TODO:
 * Hvis testen er succesfuld, gør test mere avanceret som
 * kontinuert testing og randomizing
 * Hvis ikke, slet en test og prøv igen
*/

// Workaround for line number mismatch
#line 2 "canRFtest.ino"

#include <AUnit.h>
#include <ACAN.h>
#include <SPI.h>
#include <RF24Transceiver.h>
#include <StampedCANMessage.h>
#include <BlackBox.h>
#include <TelemetryMessages.h>
#include <sys/stat.h>
#include <ctime>

using namespace std;
using aunit::TestRunner;

RF24Transceiver RFtrans;
CANMessage frame;
StampedCANMessage StpMsg;
BlackBox box;
//CanTelemetryMsg msg;

void rand_CANmsg(CANMessage *frame){
    frame->id = rand() % 2048;
    frame->rtr = rand() % 2;
    frame->len = 8;
    frame->data64 = rand() % ((2^64) - 1);
}

void edge_CANmsg(CANMessage *frame, int i){
    if (i = 0) {
        frame->id = 0x0;
        frame->len = 0;
        frame->data64 = 0x0;
    } else if (i = 1) {
        frame->id = 0x7FF;
        frame->len = 8;
        frame->data64 = 0xFFFFFFFF;
    }
    
}


ACANSettings settings (125 * 1000); // Sets wished bitrate
//-----------------------------------------------------------------------------
// Test cases
//-----------------------------------------------------------------------------

// Simple test case for testing normal pass
test(testTrue) {
    assertEqual(3, 3);
}

// Teensy 3.6 board
//-----------------------------------------------------------------------------
#ifdef TEENSY36_BOARD

// --------------------------------- setup --------------------------------- //

// Should test that both CAN0 port and CAN1 port is active
test(canX_begin) {
    const uint32_t can0_begin = ACAN::can0.begin (settings);
    const uint32_t can1_begin = ACAN::can1.begin (settings);
    assertTrue(can0_begin);
    assertTrue(can1_begin);
}

// Should test that the radio transmits a message succesfully
testing(radio_transmit) {
    char *message = "Hello test";
    RFtrans.transmit(*message, sizeof(message));
    assertStringCaseEqual("Transmission successful! ",Serial.readString());
}

// -------------------------- StampedCANMessage.h -------------------------- //

// Should test the getHeader() function
test(test_getHeader){
    assertStringCaseEqual("\"time\",\"id\",\"rtr\",\"len\",\"data\"", StpMsg.getHeader());
}

// Should test that the time stamp is correct with edge cases
test(test_StampedCANMessage_update_edge){
    CANMessage test_msg;
    for (int i = 0; i < 2; i++){
        edge_CANmsg(&test_msg, i);
        /*char tmp[200]; // Debug lines
        snprintf(tmp,200,"%d,%d,%d,%llu",test_msg.id,test_msg.rtr,test_msg.len,test_msg.data64);
        Serial.println(tmp);*/
        StpMsg.update(&test_msg);
        char test_str[200];
        snprintf(test_str,200,"\"%02d/%02d/%04d %02d-%02d-%02d\",%" PRIx16 ",%d,%d,%" PRIx64 "", 
            day(), month(), year(), hour(), minute(), second(), test_msg.id, test_msg.rtr, test_msg.len, 
            test_msg.data64);
        assertStringCaseEqual(test_str, StpMsg.toString());
    }
}

// Should test that the time stamp is correct with random values
testing(test_StampedCANMessage_update_random){
    CANMessage test_msg;
    rand_CANmsg(&test_msg);
    /*char tmp[200]; // Debug lines
    snprintf(tmp,200,"%d,%d,%d,%llu",test_msg.id,test_msg.rtr,test_msg.len,test_msg.data64);
    Serial.println(tmp);*/
    StpMsg.update(&test_msg);
    char test_str[200];
    snprintf(test_str,200,"\"%02d/%02d/%04d %02d-%02d-%02d\",%" PRIx16 ",%d,%d,%" PRIx64 "", 
        day(), month(), year(), hour(), minute(), second(), test_msg.id, test_msg.rtr, test_msg.len, 
        test_msg.data64);
    assertStringCaseEqual(test_str, StpMsg.toString());
}

// ------------------------------- BlackBox.h ------------------------------ //

// Should test that the blackbox initializes
test(test_init){
    bool t = box.init();
    assertEqual(true, t);
}


// Should test that a new log file is started
test(test_startNewLogFile){
    box.startNewLogFile();
    struct stat buffer;
    assertEqual(true, (stat ("Tjark, hvor ligger filen?", &buffer) == 0));
}

// Should test that the current log file is ended
test(test_endLogFile){
    // Find a way to check this
}

test(test_addNewLogStr){
    // Learn how to read last line in the .csv file, then do a equal string assertion
}

// --------------------------- RF24Transceiver.h --------------------------- //

// Should test that the radio receives a message succesfully
testing(test_receive) {
    RFtrans.receive();
    assertStringCaseNotEqual("", Serial.readString());
}

testing(test_transmit){

}

// Teensy 4.0 board
//-----------------------------------------------------------------------------
#else

// --------------------------- RF24Transceiver.h --------------------------- //

// Should test that the RF module initializes
test(test_RFinit){
    RFinit();
}



#endif

// ------------------------------- Fail test ------------------------------- //

// Should test that a test can fail
test(testFail) {
    assertTrue(false);
}

//-----------------------------------------------------------------------------
// setup() and loop()
//-----------------------------------------------------------------------------

void setup() {
    #if ARDUINO
        delay(2000);
    #endif
    Serial.begin(9600);
    while(!Serial){}
    pinMode(LED_BUILTIN,OUTPUT);

    ACAN::can1.begin(settings);
    ACAN::can0.begin(settings);
}

void loop() {
    aunit::TestRunner::run();
}