/*
 Copyright (c) 2014-present PlatformIO <contact@platformio.org>

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
**/

// Maybe use pio --without-building?

#include <unity.h>
#include <ACAN.h>
#include <sys/stat.h>
#include <ctime>
#include <fstream>

#include "BlackBox.h"

CANMessage frame;
BlackBox box;

// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

void test_add(void) {
    TEST_ASSERT_EQUAL(32, 25 + 7);
}

void test_subtract(void) {
    TEST_ASSERT_EQUAL(20, 23 - 3);
}

void test_multiplication(void) {
    TEST_ASSERT_EQUAL(50, 25 * 2);
}

void test_division(void) {
    TEST_ASSERT_EQUAL(32, 96 / 3);
}

// Should test that both CAN0 port and CAN1 port is active
void test_can_x_begin(void) {
    ACANSettings settings (125 * 1000); // Sets wished bitrate
    const uint32_t can0_begin = ACAN::can0.begin (settings);
    const uint32_t can1_begin = ACAN::can1.begin (settings);
    TEST_ASSERT_TRUE(can0_begin);
    TEST_ASSERT_TRUE(can1_begin);
}

// ------------------------------- BlackBox.h ------------------------------ //

// Should test that the blackbox initializes
void test_init(void){
    bool t = box.init();
    TEST_ASSERT_TRUE(t);
}

void test_getTeensy3Time(void){
    char test_str[200];
    snprintf(test_str,200,"\"%02d/%02d/%04d %02d-%02d-%02d\",%" PRIx16 ",%d,%d,%" PRIx64 "", 
    day(), month(), year(), hour(), minute(), second(), frame.id, frame.rtr, frame.len, 
    frame.data64);
    // Forkerte typer, men basale princip er nedenstående
    TEST_ASSERT_INT_WITHIN(1, test_str, box.getTeensy3Time);
}

void test_updateFileName(void){
    /*String oldname = BlackBox::fileName;
    delay(1000);
    box.updateFileName();
    String newname = BlackBox::fileName;
    assertEqual(newname, "log__%02d_%02d_%02d__%02d_%02d_%04d.csv", hour(), minute(), second(),
    day(), month(), year())
    assertNotEqual(oldname, newname);*/
}

// Should test that a new log file is started
void test_startNewLogFile(void){
    /*box.startNewLogFile();
    struct stat buffer;
    assertEqual(true, (stat ("Tjark, hvor ligger filen?", &buffer) == 0));*/
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

void process() {
    UNITY_BEGIN();
    RUN_TEST(test_add);
    RUN_TEST(test_subtract);
    RUN_TEST(test_multiplication);
    RUN_TEST(test_division);
    RUN_TEST(test_can_x_begin);
    RUN_TEST(test_init);
    UNITY_END();
}

#ifdef ARDUINO

#include <Arduino.h>
void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);
    ACANSettings settings (125 * 1000); // Sets wished bitrate
    ACAN::can0.begin (settings);
    ACAN::can1.begin (settings);

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
