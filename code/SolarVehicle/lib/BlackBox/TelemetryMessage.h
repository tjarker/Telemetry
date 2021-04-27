#ifndef __TELEMETRYMESSAGE_H__
#define __TELEMETRYMESSAGE_H__

#include <Arduino.h>

#ifdef TEENSY36_BOARD

#include <ACAN.h>
#include <TimeLib.h>

#endif

enum CMD: uint8_t {
  RECEIVED_CAN = 0x00, 
  BROADCAST_CAN = 0x01,
  START_LOGGING = 0x02, 
  STOP_LOGGING = 0x03
};

class TelemetryMessage {
    public:
      CMD cmd;          // offset = 0x00:      opcode of the message

      // data payload
      uint16_t id;      // offset = 0x01-0x02: identifier accompanying the CAN frame
      uint8_t rtr;      // offset = 0x03:      indicator of whether the frame is requesting data from another device
      uint8_t len;      // offset = 0x04:      indicates how many of the payload bytes contain valid data
      union{
        uint8_t data[8];// offset = 0x05-0x0D: the data payload
        uint64_t data64;
      };
      uint8_t s;        // offset = 0x0E:      the second when the frame was received
      uint8_t m;        // offset = 0x0F:      the minute when the frame was received
      uint8_t h;        // offset = 0x10:      the hour when the frame was received


    public: TelemetryMessage(){
        // nothing
    }

    public: TelemetryMessage(uint16_t id, uint8_t len, uint64_t data){
        this->id = id;
        this->len = len;
        this->data64 = data;
    }

    public: ~TelemetryMessage(){
        // nothing
    }

    public: void randomize(){
      this->id = random(0,0x400);
      this->rtr = random(0,2);
      this->len = random(0,8);
      for(uint8_t i = 0; i < 8; i++){
          this->data[i] = random(0,0x100);
      }
      this->s = random(0,61);
      this->m = random(0,61);
      this->h = random(0,25);
    }

    public: const String toString(){
        char tmp[200];
        snprintf(tmp,200,"\"%02d-%02d-%02d\",%u,%u,%u,%llu",h,m,s,id,rtr,len,data64);
        return String(tmp);
    }

    public: uint32_t toString(char *buf, uint32_t len){
        return snprintf(buf,len,"\"%02d-%02d-%02d\",%u,%u,%u,%llu",h,m,s,id,rtr,len,data64);
    }

    static const String getHeader(){
        return "\"time\",\"id\",\"rtr\",\"len\",\"data\"";
    }

    #ifdef TEENSY36_BOARD

    public: TelemetryMessage(CANMessage *msg){
        id = msg->id;
        rtr = msg->rtr;
        len = msg->len;
        data64 = msg->data64;
        stamp();
    }
    public: void update(CANMessage *msg){
        id = msg->id;
        rtr = msg->rtr;
        len = msg->len;
        data64 = msg->data64;
        stamp();
    }

    public: void stamp(){
        s = second();
        m = minute();
        h = hour();
    }

    #else


    public: const String toJSON(){
      if(this->cmd == CMD::RECEIVED_CAN){
        char tmp[200];
        snprintf(tmp,200,"{\"cmd\":%d,\"can\":{\"id\":%d,\"rtr\":%s,\"len\":%d,\"data\":%llu,\"stamp\":{\"hour\":%d,\"minute\":%d,\"second\":%d}}}\n",cmd,id,rtr ? "true" : "false",len,data64,h,m,s);
        return String(tmp);
      } else {
        char tmp[50];
        snprintf(tmp,50,"{\"cmd\":%d,\"can\":null",cmd);
        return String(tmp);
      }
    }
    

    #endif

};

#endif