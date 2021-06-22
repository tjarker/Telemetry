#include <stdint.h>
#include <stdio.h>

typedef enum CMD: uint8_t {
  RECEIVED_CAN = 0x00, 
  BROADCAST_CAN = 0x01,
  START_LOGGING = 0x02, 
  STOP_LOGGING = 0x03,
  FILE_STREAM_START = 0x04,
  FILE_STREAM_STOP = 0x05,
  FILE_STREAM_DATA = 0x06
} cmd_t;

class BaseTelemetryMsg {

  public: cmd_t cmd;
  public: uint8_t data[31];

  public: uint32_t toString(char *buf, uint32_t len){
    uint32_t curPos = snprintf(buf,len,"%d:",cmd);
    for(uint8_t i = 0; i < 31; i++){
      curPos += snprintf(&(buf[curPos]),len-curPos," %x",data[i]);
    }
    return curPos;
  }
};