/**
 * This file contains all the message types used in the telemetry system.
 * 
 * All messages have a length of 32 bytes where the first byte is encoding the
 * type of the message.
 * 
 * All messages can be casted to the BaseTelemetryMsg which treats the data
 * payload as raw bytes.
 * 
 * Message types are:
 *  - CanTelemetryMsg:  Contains a CAN frame and a time stamp
 *  - FileStreamMsg:    Can be used to stream characters from a file
 */

#ifndef __TELEMETRYMESSAGES_H__
#define __TELEMETRYMESSAGES_H__

#include <Arduino.h>
#include <ChRt.h>

#ifdef TEENSY36_BOARD

#include <ACAN.h>
#include <TimeLib.h>

#endif

/**
 * This type is used for indentifying message types
 */
typedef enum CMD: uint8_t {
  RECEIVED_CAN = 0x00, 
  BROADCAST_CAN = 0x01,
  START_LOGGING = 0x02, 
  STOP_LOGGING = 0x03,
  START_STREAMING = 0x04, 
  STOP_STREAMING = 0x05,
  CONNECT_CAN = 0x06,
  DISCONNECT_CAN = 0x07
} cmd_t;

/**
 * The base type for all messages treating the payload data as raw bytes
 */
class BaseTelemetryMsg {

  public: cmd_t cmd;
  public: uint8_t data[31];

  public: uint32_t toString(char *buf, uint32_t len){
    uint32_t curPos = snprintf(buf,len,"%02x:",cmd);
    for(uint8_t i = 0; i < 31; i++){
      curPos += snprintf(&(buf[curPos]),len-curPos," %02x",data[i]);
    }
    return curPos;
  }

  public: uint8_t* toBytes(){return (uint8_t*)this;}

  public: void randomize(){
      this->cmd = (cmd_t) random(0, 0x99);   
      for (int i = 0; i < 31; i++){
        this->data[i] = random(0,0x99); 
      }  
  }

}__attribute__((packed));

/**
 * A message type used to stream characters from a file
 */
class FileStreamMsg {
  public: cmd_t cmd;
  public: char str[31];

  public: const String toString(){
    char tmp[31+4];
    snprintf(tmp,31+4,"FS: %s",str);
    return String(tmp);
  }

  public: BaseTelemetryMsg* toMessage(){return (BaseTelemetryMsg*)this;}

}__attribute__((packed));

/**
 * A message type used to send a CAN frame and a associated time stamp
 */
class CanTelemetryMsg {
    public:
      cmd_t cmd;        // offset = 0x00:      type of the message
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


    public: CanTelemetryMsg(){
        // nothing
    }

    public: CanTelemetryMsg(uint16_t id, uint8_t len, uint64_t data){
        this->id = id;
        this->len = len;
        this->data64 = data;
    }

    public: ~CanTelemetryMsg(){
        // nothing
    }

    public: BaseTelemetryMsg* toMessage(){return (BaseTelemetryMsg*)this;}
    

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

    public: uint32_t toString(char *buf, uint32_t len, uint8_t base){
        uint32_t strLength;
        if(base == 16){
          strLength = snprintf(buf,len,"\"%02d-%02d-%02d\",%X,%X,%X,%llX",h,m,s,id,rtr,this->len,data64);
        } else {
          strLength = snprintf(buf,len,"\"%02d-%02d-%02d\",%d,%d,%d,%lld",h,m,s,id,rtr,this->len,data64);
        }
        return strLength;
    }

    public: uint32_t toString(char *buf, uint32_t len){
        return snprintf(buf,len,"\"%02d-%02d-%02d\",%u,%u,%u,%llu",h,m,s,id,rtr,this->len,data64);
    }

    static const String getHeader(){
        return "\"time\",\"id\",\"rtr\",\"len\",\"data\"";
    }

    #ifdef CAN_ADD_TIME_STAMPS

    public: CanTelemetryMsg(CANMessage *msg){
        id = msg->id;
        rtr = msg->rtr;
        len = msg->len;
        data64 = msg->data64;
        stamp();
    }
    public: void update(CANMessage *msg){
      chSysLock();
      this->id = msg->id;
      this->rtr = msg->rtr;
      this->len = msg->len;
      this->data64 = msg->data64;
      stamp();
      chSysUnlock();
    }

    public: void toCanFrame(CANMessage *msg){
      chSysLock();
      memcpy(&(msg->data),&data,8);
      msg->id = id;
      msg->rtr = rtr;
      msg->len = len;
      msg->ext = false;
      chSysUnlock();
    }

    public: void stamp(){
        s = second();
        m = minute();
        h = hour();
    }

    #endif

    #ifdef CAN_SERIALIZE_TO_JSON
    public: const String toJSON(char *buf, uint32_t len){
      return snprintf(buf,len,"{\"cmd\":%d,\"can\":{\"id\":%d,\"rtr\":%s,\"len\":%d,\"data\":%llu,\"stamp\":{\"hour\":%d,\"minute\":%d,\"second\":%d}}}\n",cmd,id,rtr ? "true" : "false",this->len,data64,h,m,s);
    }
    

    #endif

}__attribute__((packed));

#endif