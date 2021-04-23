/* CAN bus and RF communication
 *
 * @author Victor Alexander Hansen
 * @author Steffan Martin Kunoy
 * @author Tjark Petersen
 * 
 * For CAN instructions for Teensy board, see https://github.com/pierremolinaro/acan
 * For DTU Roadrunners previous use of Teensy, see https://bitbucket.org/dtucar/autonomous-ecocar/src/master/Source/
 * For more specific examples of how the CAN was implemented in the Ecocar, see
 * https://bitbucket.org/dtucar/ecocar/src/master/Code/CanLib/canbus.cpp
 * https://bitbucket.org/dtucar/ecocar/src/master/Code/CanLib/canbus.h
 * https://bitbucket.org/dtucar/ecocar/src/master/Code/CanLib/canbusKeywords.h
*/

/* TODO:
 * Kan payload til antenne effektiviseres? E.g. kan vi skrive en funktion som
 * reducerer størrelsen af payload, men som stadig aflæses korrekt ved ankomst, ek-
 * sempelvis ved en bit i front som er størrelse på payload?
 * 
 * Add code for RF transmission, possibly new file
 * 
 * Make a test, https://docs.platformio.org/en/latest/plus/unit-testing.html
*/

#include "canRF.h"

void setCan() {
    ACANSettings settings (125 * 1000); // Sets wished bitrate
    ACAN::can1.begin(settings);
    ACAN::can0.begin(settings);
}

// Function for transmitting message from RF24 to CAN
void RFtoCAN (uint32_t payloadR){
    frame.id = ((0x11 << 16) & payloadR); // Identifier
    frame.rtr = 0; // Remote Transmit Request, 0 = data frame, 1 = request frame
    frame.len = 8; // Data Lenght Code, sets the length of Data field

    frame.data[0] = 0x11 & payloadR;
    frame.data[1] = (0x11 << 2) & payloadR;
    frame.data[2] = (0x11 << 4) & payloadR;
    frame.data[3] = (0x11 << 6) & payloadR;
    frame.data[4] = (0x11 << 8) & payloadR;
    frame.data[5] = (0x11 << 10) & payloadR;
    frame.data[6] = (0x11 << 12) & payloadR;
    frame.data[7] = (0x11 << 14) & payloadR;

    //ACAN::can0.tryToSend(frame);
}

// Transforms a CAN message to antenna payload on the form
// CanId,canData[7][6][5][4][3][2][1][0]
String CANtoRF () {
    int payloadT = 0x0;
    int n = 0;

    if (ACAN::can1.available()) {
        ACAN::can1.receive(frame);
    }
    // Transforms canData to one integer
    while(n < 8){
        payloadT += (frame.data[n] << (n*2));
        n++;
    }
    // Adds the canID to the most significant bits of the integer
    payloadT += (frame.id << n*2);
    char tmp[200];
    String t = snprintf(tmp, 200, "%d", payloadT);

    return t;
}