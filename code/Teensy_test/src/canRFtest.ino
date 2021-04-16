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
using aunit::TestRunner;

#include <canRF.h>
CANbus bus;

//----------------------------------------------------------------------------
// Test cases
//----------------------------------------------------------------------------

// Simple test case for testing normal pass
test(testTrue) {
    assertEqual(3, 3);
}

test(canX_begin) {
    const uint32_t can0_begin = ACAN::can0.begin (settings);
    const uint32_t can1_begin = ACAN::can1.begin (settings);
    assertTrue(can0_begin);
    assertTrue(can1_begin);
}

/* Ophæv udkommentering hvis øvrige tests virker fint
test(RFtoCan) {
    uint32_t test_id = 1;
    bus.RFtoCAN(0x10000A234);
    assertEqual(frame.data[0], 0x4);
    assertEqual(frame.data[1], 0x3);
    assertEqual(frame.data[2], 0x2);
    assertEqual(frame.data[3], 0xA);
    assertEqual(frame.id, test_id);
}
*/

/* Ophæv udkommentering hvis øvrige tests virker fint
test(CANtoRF) {
    uint32_t des_payload = 0x3F00FF000470005FAD;
    frame.id = 0x3F;
    frame.data[0] = 0xAD;
    frame.data[1] = 0x5F;
    frame.data[2] = 0x0;
    frame.data[3] = 0x70;
    frame.data[4] = 0x04;
    frame.data[5] = 0x0;
    frame.data[6] = 0xFF;
    frame.data[7] = 0x0;

    uint32_t payloadT = bus.CANtoRF();
    assertEqual(payloadT, des_payload);
}
*/

test(testFail) {
    // Test is supposed to fail
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