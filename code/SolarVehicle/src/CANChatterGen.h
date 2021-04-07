#include <ACAN.h>

class CANChatterGen {

    private: CANMessage msg;

    private: uint32_t counter = 0;

    public: CANChatterGen(){
        msg.ext = false;
        msg.rtr = false;
        msg.len = 8;
    }
    public: ~CANChatterGen(){}

    public: CANMessage newChatter(){
        for(uint8_t i = 0; i < 8; i++){
            msg.data[i] = counter;
        }
        msg.id = counter++;
        return msg;
    }
    public: void reverse(){
        counter--;
    }
};