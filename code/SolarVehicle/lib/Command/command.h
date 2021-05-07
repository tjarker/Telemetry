#ifndef COM_H
#define COM_H
#include <Arduino.h>

class Command{
    bool log;
    bool reset;
    uint8_t filter_id;
    uint8_t mask;
    public:
        void Default();
        void Log_toggle();
        void Reset_log();
        void Filter(uint8_t filter_id, uint8_t mask);
};

#endif

/* Pseudo code
 * // mainTeensy36.cpp
 * ...
 * Command cmd;
 * 
 * void setup(){
 *  cmd.Default();
 *  ...
 * }
 * 
 * void loop(){
 *  cmd.log = (&jsonString & (0x11 << 14))
 *  cmd.reset = (&jsonString & (0x11 << 16))
 * 
 *  if (!cmd.log) {
 *      cmd.Log_toggle();
 *  }
 *  if (cmd.reset) {
 *      cmd.Reset_log();
 *  }
 *  ...
 * }
 * 
 * // RF24Transceiver.cpp
 * void RF24Transceiver::transmit(char &message, int size)
 * {
 *  uint8_t filter_id = (&jsonString & 0x11 << 12)
 *  uint8_t mask = (&jsonString & 0x11 << 14)
 *  cmd.Filter(filter_id, mask);
 * 
 *  m_radio->stopListening();
 *  ...
*/