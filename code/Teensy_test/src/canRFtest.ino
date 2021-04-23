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
 * 
 * Tilføj flere tests for RF transceiver og BB
*/

// Workaround for line number mismatch
#line 2 "canRFtest.ino"

#include <AUnit.h>
#include <ACAN.h>
#include <time.h>
#include <string>
#include "RF24Transceiver.h"
#include "StampedCANMessage.h"


using namespace std;
using aunit::TestRunner;

RF24Transceiver RFtrans;
StampedCANMessage CANmsg;

ACANSettings settings (125 * 1000); // Sets wished bitrate
//----------------------------------------------------------------------------
// Test cases
//----------------------------------------------------------------------------

// Simple test case for testing normal pass
test(testTrue) {
    assertEqual(3, 3);
}

// Should test that both CAN0 port and CAN1 port is active
test(canX_begin) {
    const uint32_t can0_begin = ACAN::can0.begin (settings);
    const uint32_t can1_begin = ACAN::can1.begin (settings);
    assertTrue(can0_begin);
    assertTrue(can1_begin);
}

// Should test that the time stamp is correct
testing(time_stamp) {
    char tmp[200];
    String time = String(snprintf(tmp, 200, "\"%02d/%02d/%04d %02d-%02d-%02d\",%d,%d,%d,%d",
        day(), month(), year(), hour(), minute(), second(), CANmsg.id,
        CANmsg.rtr, CANmsg.len, CANmsg.data64));
    assertStringCaseEqual(time, CANmsg.toString());
}

// Should test that the radio transmits a message succesfully
testing(radio_transmit) {
    char *message = "Hello test";
    RFtrans.transmit(*message, sizeof(message));
    assertStringCaseEqual("Transmission successful! ",Serial.readString());
}

// Should test that the radio receives a message succesfully
testing(radio_receive) {
    RFtrans.receive();
    assertStringCaseNotEqual("", Serial.readString());
}

// Should test that a test can fail
test(testFail) {
    assertTrue(false);
}

//----------------------------------------------------------------------------
// setup() and loop()
//----------------------------------------------------------------------------

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