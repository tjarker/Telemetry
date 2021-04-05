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

#define BIT_RATE 125 * 1000;
ACANSettings settings (125 * 1000); // Sets wished bitrate

class CANbus {

public:
    CANbus() {}
    void setCan();
    void RFtoCAN(uint32_t payloadR);
    uint32_t CANtoRF ();
};

#endif