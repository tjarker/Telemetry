#ifndef COM_H
#define COM_H
#include <Arduino.h>

class Command{
    bool log;
    bool reset;
    public:
        void Default();
        void Reset_log();
        void Filter(uint8_t filter_id, uint8_t mask);
};

#endif