/* Test for the CAN files
 * Using AUnit library, see the following for more info
 * https://github.com/bxparks/AUnit
 * https://www.arduino.cc/reference/en/libraries/aunit/
 * https://docs.microsoft.com/en-us/visualstudio/test/writing-unit-tests-for-c-cpp?view=vs-2019
 * 
 * Please work ffs
*/

// Workaround for line number mismatch
#line 2 "canRFtest.ino"

#include <AUnit.h>
using aunit::TestRunner;

#include <ACAN.h>

test(testTrue) {
    assertEqual(3, 3);
}

test(canX_begin) {
    ACANSettings settings (125 * 1000);
    assertTrue(ACAN::can0.begin (settings));
    assertTrue(ACAN::can1.begin (settings));
}

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
}

void loop() {
    aunit::TestRunner::run();
}