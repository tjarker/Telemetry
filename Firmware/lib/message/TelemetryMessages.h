/**
 * @file    TelemetryMessages.h
 * @author  Tjark Petersen
 * @brief   Declaration of BaseTelemetryMsg and CanTelemetryMsg class
 * @note    This file contains all the message types used in the telemetry system.
 * 
 *          All messages have a length of 32 bytes where the first byte is encoding the
 *          type of the message.
 * 
 *          All messages can be casted to the BaseTelemetryMsg which treats the data
 *          payload as raw bytes.
 * 
 *          Message types are:
 *            - CanTelemetryMsg:  Contains a CAN frame and a time stamp
 *            - FileStreamMsg:    Can be used to stream characters from a file
*/

#ifndef __TELEMETRYMESSAGES_H__
#define __TELEMETRYMESSAGES_H__
#include <Arduino.h>
#include <ChRt.h>

#ifdef TEENSY36_BOARD

#include <ACAN.h>
#include <TimeLib.h>

#endif

#define BYTES_PER_MESSAGE 16    // All message types are 16 bytes long

/**
 * @brief   enum typedef for identifying message commands.
*/
typedef enum CMD: uint8_t 
{
  RECEIVED_CAN = 0x00,        // Message received from CAN bus. 
  BROADCAST_CAN = 0x01,       // Message to be transmitted to CAN bus.
  START_LOGGING = 0x02,       // Start logging to the black box memory. 
  STOP_LOGGING = 0x03,        // Stop logging to the black box memory. 
  START_STREAMING = 0x04,     // Start streaming data to the Support Vehicle Unit.
  STOP_STREAMING = 0x05,      // Stop streaming data to the Support Vehicle Unit.
  SLEEP = 0x06,               // Suspend streaming and logging.
  WAKE_UP = 0x07,             // Recommence streaming and logging.
  ENABLE_ENCRYPTION = 0x08,   // Activate RSA encryption of transmitted messages. 
  DISABLE_ENCRYPTION = 0x09   // Deactivate RSA encryption of transmitted messages. 
} cmd_t;

/**
 * @brief   The base type for all messages, treating the payload data as raw bytes. 
*/
class BaseTelemetryMsg 
{
  /* <!-- Public member variables -->*/
  public: cmd_t cmd;
  public: uint8_t data[BYTES_PER_MESSAGE-1];
  public: static uint32_t length(){return BYTES_PER_MESSAGE;}

  /* <!-- Public member functions  -->*/
  /**
   * @brief   Represent BaseTelemetryMsg as formatted string.
   * @param   buf 
   * @param   len 
   * @return  
  */
  public: uint32_t toString(char *buf, uint32_t len)
  {
    uint32_t curPos = snprintf(buf,len,"%02x:",cmd);
    for(uint8_t i = 0; i < BYTES_PER_MESSAGE-1; i++){
      curPos += snprintf(&(buf[curPos]),len-curPos," %02x",data[i]);
    }
    return curPos;
  }

  /**
   * @brief   Randomizes data fields of BaseTelemetryMsg. 
   * @param   None 
   * @return  void
  */
  public: void randomize()
  {
    this->cmd = (cmd_t) random(0, 0x99);   
    for (int i = 0; i < 31; i++){
      this->data[i] = random(0,0x99); 
    }  
  }

}__attribute__((packed));

/**
 * @brief A message type used to send a CAN frame and an associated time stamp
 */
class CanTelemetryMsg 
{
  /* <!-- Public member variables --> */
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

  /* <!-- Private member functions --> */
  /**
   * @brief   CanTelemetryMsg constructor function. 
   * @param   None
  */ 
  public: CanTelemetryMsg()
  {
    // nothing
  }

  /**
   * @brief   Overloaded CanTelemetryMsg constructor function.  
   * @param   id the CAN id
   * @param   len the length of the CAN frame
   * @param   data the data payload
  */
  public: CanTelemetryMsg(uint16_t id, uint8_t len, uint64_t data)
  {
    this->id = id;
    this->len = len;
    this->data64 = data;
  }

  /**
   * @note    Not currently used. 
  */ 
  public: ~CanTelemetryMsg()
  {
    // nothing
  }

  /**
   * @return  Length of CanTelemetryMsg. 
  */ 
  public: static uint32_t length(){return BYTES_PER_MESSAGE;}

  /**
   * @return  A pointer to the message typecast as a BaseTelemetryMsg.  
  */ 
  public: BaseTelemetryMsg* toMessage(){return (BaseTelemetryMsg*)this;}
  
  /**
   * @brief  Randomizes all data and time stamp fields in the CanTelemetryMsg. 
  */ 
  public: void randomize()
  {
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

  /**
   * @return  CanTelemetryMsg header string.  
  */ 
  static const String getHeader()
  {
    return "\"time\",\"id\",\"rtr\",\"len\",\"data\"";
  }

  #ifdef CAN_ADD_TIME_STAMPS  // Required build-flag in platform.ini file

  /**
   * @brief   Third overloaded CanTelemetryMsg constructor function.  
  */ 
  public: CanTelemetryMsg(CANMessage *msg){
        id = msg->id;
        rtr = msg->rtr;
        len = msg->len;
        data64 = msg->data64;
        stamp();
  }
  
  /**
   * @brief   Function to update CanTelemetryMsg fields. 
   * @param   msg Pointer to CANMessage variable.
   * @return  void 
  */ 
  public: void update(CANMessage *msg){
      chSysLock();
      this->id = msg->id;
      this->rtr = msg->rtr;
      this->len = msg->len;
      this->data64 = msg->data64;
      stamp();
      chSysUnlock();
  }

  /**
   * @brief   ?
   * @param   msg Pointer to CANMessage variable. 
   * @return  void
  */ 
  public: void toCanFrame(CANMessage *msg){
      chSysLock();
      memcpy(&(msg->data),&data,8);
      msg->id = id;
      msg->rtr = rtr;
      msg->len = len;
      msg->ext = false;
      chSysUnlock();
  }

  /**
   * @brief   Sets the CanTelemetryMsg time stamp to the current system time. 
  */ 
  public: void stamp()
  {
    s = second();
    m = minute();
    h = hour();
  }

  /**
   * @brief   toString method with day-month-year tag
   * @param   buf the target buffer
   * @param   len the target buffer length
   * @return  the number of bytes written to the buffer
  */ 
  public: uint32_t toString(char *buf, uint32_t len)
  {
    return snprintf(buf,len,"\"%02d/%02d/%4d %02d-%02d-%02d\",%u,%u,%u,%llu",day(),month(),year(),h,m,s,id,rtr,this->len,data64);
  }
  #else
  
  /**
   * @brief   toString method
   * @param   buf the target buffer
   * @param   len the target buffer length
   * @return  the number of bytes written to the buffer
  */  
  public: uint32_t toString(char *buf, uint32_t len)
  {
    return snprintf(buf,len,"\"%02d-%02d-%02d\",%u,%u,%u,%llu",h,m,s,id,rtr,this->len,data64);
  }

  #endif

  #ifdef CAN_SERIALIZE_TO_JSON  // Required build-flag in platform.ini file
  
  /**
   * @brief   Method to represent CanTelemetryMsg as JSON string. 
   * @param   buf Buffer to write JSON string to. 
   * @param   len Maximum number of bytes to write. 
   * @return  CanTelemetryMsg formatted as JSON string. 
  */
  public: const String toJSON(char *buf, uint32_t len)
  {
    return snprintf(buf,len,"{\"cmd\":%d,\"can\":{\"id\":%d,\"rtr\":%s,\"len\":%d,\"data\":%llu,\"stamp\":{\"hour\":%d,\"minute\":%d,\"second\":%d}}}\n",cmd,id,rtr ? "true" : "false",this->len,data64,h,m,s);
  }
    
  #endif

}__attribute__((packed));

#endif