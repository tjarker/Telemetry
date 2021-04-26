#ifndef __LOGTYPE_H__
#define __LOGTYPE_H__
#include <Arduino.h>


#ifdef TEENSY36

#include <ACAN.h>
#include <TimeLib.h>

class StampedCANMessage {
    public:
        uint16_t id;
        uint8_t rtr;
        uint8_t len;
        union {
            uint64_t data64        ; // Caution: subject to endianness
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
        snprintf(tmp,200,"\"%02d/%02d/%04d %02d-%02d-%02d\",%" PRIx16 ",%d,%d,%" PRIx64 "",d,mon,y,h,m,s,id,rtr,len,data64);
        return String(tmp);
    }
    public: uint32_t toString(char *buf){
        return snprintf(buf,200,"\"%02d/%02d/%04d %02d-%02d-%02d\",%" PRIu16 ",%d,%d,%" PRIu64 "",d,mon,y,h,m,s,id,rtr,len,data64);
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

#else

#ifdef ARDUINOCODE

class StampedCANMessage {
    public:
        uint16_t id;
        uint8_t rtr;
        uint8_t len;
        union {
            uint64_t data64        ; // Caution: subject to endianness           
            uint8_t  data   [8] = {0, 0, 0, 0, 0, 0, 0, 0} ;
        };
        uint8_t s;
        uint8_t m;
        uint8_t h;
        uint8_t d;
        uint8_t mon;
        uint8_t y;

    public: const String toString(){
        char tmp[200];
        snprintf(tmp,200,"\"%02d/%02d/%04d %02d-%02d-%02d\",%d,%d,%d,%llu",d,mon,y,h,m,s,id,rtr,len,data64);
        return String(tmp);
    }
};

#else

class StampedCANMessage {
    public:
        uint16_t id;
        uint8_t rtr;
        uint8_t len;
        union {
            uint64_t data64; // Caution: subject to endianness
            uint8_t  data   [8] = {0, 0, 0, 0, 0, 0, 0, 0} ;
        };
        uint8_t s;
        uint8_t m;
        uint8_t h;
        uint8_t d;
        uint8_t mon;
        uint8_t y;

    public: StampedCANMessage() {
        this->id = random(0,0x400);
        this->rtr = random(0,2);
        for(uint8_t i = 0; i < 8; i++){
            this->data[i] = random(0,0x100);
        }
        this->s = random(0,61);
        this->m = random(0,61);
        this->h = random(0,25);
        this->d = random(0,32);
        this->mon = random(0,13);
        this->y = 21;
    }

    public: StampedCANMessage(uint16_t id, uint8_t len, uint64_t data){
        this->id = id;
        this->len = len;
        this->data64 = data;
    }

    public: const String toString(){
        char tmp[200];
        snprintf(tmp,200,"\"%02d/%02d/%04d %02d-%02d-%02d\",%" PRIu16 ",%d,%d,%" PRIu64 "",d,mon,y,h,m,s,id,rtr,len,data64);
        return String(tmp);
    }
};

#endif
#endif

#endif