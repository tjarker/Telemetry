#include <Arduino.h>
#include <ACAN.h>


class StampedCANMessage {
    public:
        uint16_t id;
        uint8_t rtr;
        uint8_t len;
        union {
            uint64_t data64        ; // Caution: subject to endianness
            uint32_t data32 [2]    ; // Caution: subject to endianness
            uint16_t data16 [4]    ; // Caution: subject to endianness
            float    dataFloat [2] ; // Caution: subject to endianness
            uint8_t  data   [8] = {0, 0, 0, 0, 0, 0, 0, 0} ;
        };
        uint8_t s;
        uint8_t m;
        uint8_t h;
        uint8_t d;
        uint8_t mon;
        uint8_t y;

    public: StampedCANMessage(CANMessage *msg){
        id = msg->id;
        rtr = msg->rtr;
        len = msg->len;
        data64 = msg->data64;
        stamp();
    }

    public: StampedCANMessage(){
        // nothing
    }

    public: ~StampedCANMessage(){
        // nothing
    }

    public: void update(CANMessage *msg){
        id = msg->id;
        rtr = msg->rtr;
        len = msg->len;
        data64 = msg->data64;
        stamp();
    }
        
    public: const String toString(){
        char tmp[200];
        snprintf(tmp,200,"\"%02d/%02d/%04d %02d-%02d-%02d\",%" PRIu16 ",%d,%d,%" PRIu64 "",
        d,mon,y,h,m,s,id,rtr,len,data64);
        return String(tmp);
    }
    public: uint32_t toString(char *buf){
        snprintf(buf,200,"\"%02d/%02d/%04d %02d-%02d-%02d\",%" PRIu16 ",%d,%d,%" PRIu64 "",
        d,mon,y,h,m,s,id,rtr,len,data64);
    }

    public: void stamp(){
        s = second();
        m = minute();
        h = hour();
        d = day();
        mon = month();
        y = year();
    }

        static const String getHeader(){
            return "\"time\",\"id\",\"rtr\",\"len\",\"data\"";
        }
};