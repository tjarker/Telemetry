/* CAN bus C++ transmission receiving file
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

#include <Arduino.h>
#include <ACAN.h>


CANMessage frame;

// Function for transmitting message through CAN0
// Pre condition: CAN identifier must be specified by user
// Post condition: transmits message saying "HELLO, x"
void CANtransmit (uint8_t CanId){
    frame.id = CanId; // Identifier, lower number = higher priority
    frame.rtr = 0; // Remote Transmit Request, 0 = data frame, 1 = request frame
    frame.len = 8; // Data Lenght Code, sets the length of Data field

    // Actual message
    frame.data[0] = 'H';
    frame.data[1] = 'E';
    frame.data[2] = 'L';
    frame.data[3] = 'L';
    frame.data[4] = 'O';
    frame.data[5] = ',';
    frame.data[6] = ' ';
    frame.data[7] = CanId;

    ACAN::can0.tryToSend(frame);
}

// Function for receiving message through CAN1
// Pre condition: Listens for a data frame to receive
// Post condition: Prints received data fram to serial monitor
bool CANreceive(){
    const bool ok = ACAN::can1.receive(frame);
    for (int i = 0; i < 8; i++){
        Serial.print(frame.data[i]);
        Serial.print(" ");
    }
    return ok;
}

void setup(){
    Serial.begin(9600); // Baud rate
    ACANSettings settings (125 * 1000); // Sets wished bitrate
    ACAN::can1.begin(settings);
    ACAN::can0.begin(settings);
}

void loop(){
    CANtransmit(0);
    if (CANreceive()){
        Serial.print("Succes");
    }
}
