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

char* CANtoRF () {
    uint32_t payloadT = 0x0;
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

    return std::sprintf(payloadT);
}

#endif