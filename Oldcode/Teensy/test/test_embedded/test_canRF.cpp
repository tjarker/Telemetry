/* 
 * @author Victor Alexander Hansen
 * @author Steffan Martin Kunoy
 * @author Tjark Petersen
 *
 * Verification for the canRF.cpp
*/

#include <Arduino.h>
#include <unity.h>
#include <canRF.h>

CANbus can;

#ifdef UNIT_TEST

// Checks that CAN ports are ready
void test_canX_begin() {
    TEST_ASSERT_TRUE_MESSAGE(ACAN::can0.begin (settings), "CAN0 not ready!");
    TEST_ASSERT_TRUE_MESSAGE(ACAN::can1.begin (settings), "CAN1 not ready!");
}

// Checks that a test can fail
void test_fail() {
    TEST_ASSERT_TRUE_MESSAGE(false, "Test should fail");
}

void setup() {
    // Waits for 2 seconds, due to lack of software reset via Serial
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_fail);
    RUN_TEST(test_canX_begin);
    UNITY_END();
}

void loop() {
    Serial.print("Testing test_canRF.cpp");
    delay(100);
}

#endif