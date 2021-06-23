/*
 @author Victor Alexander Hansen
 @author Steffan Martin Kunoy
 @author Tjark Petersen
*/

#include <ACAN.h>
#include <SPI.h>

#ifndef CANRF_H
#define CANRF_H

CANMessage frame;

void setCan(int bitrate) {
    ACANSettings settings (bitrate); // Sets wished bitrate
    ACAN::can1.begin(settings);
    ACAN::can0.begin(settings);
}

// Function for transmitting message from RF24 to CAN
void RFtoCAN (char* payloadR){
    frame.id = ((0x11 << 16) & atoi(payloadR)); // Identifier
    frame.rtr = 0; // Remote Transmit Request, 0 = data frame, 1 = request frame
    frame.len = 8; // Data Lenght Code, sets the length of Data field

    frame.data[0] = 0x11 & atoi(payloadR);
    frame.data[1] = (0x11 << 2) & atoi(payloadR);
    frame.data[2] = (0x11 << 4) & atoi(payloadR);
    frame.data[3] = (0x11 << 6) & atoi(payloadR);
    frame.data[4] = (0x11 << 8) & atoi(payloadR);
    frame.data[5] = (0x11 << 10) & atoi(payloadR);
    frame.data[6] = (0x11 << 12) & atoi(payloadR);
    frame.data[7] = (0x11 << 14) & atoi(payloadR);

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

#endif