[back to overview](../meetingNotes.md)
# Meeting Friday 26.02.2021 ROAST

## Where: Online

## Participants:
 - Tjark
 - Steffan
 - Victor
 - Christian Kampp Kruuse

## Agenda:
 - Catch up after Easter Break
 - Verification/unit testing of CAN-module
 - Interface between CAN and RF modules
   - How can RF module access CAN data stream? 
   - Do we use a global buffer based on standard C++ libraries or another solution?

## TODO: 
 - Implement ChibiOS mailbox system to maintain read/write buffers 
 - Datalogger class can write to FIFO buffer on Teensy
