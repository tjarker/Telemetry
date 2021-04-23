#include <Arduino.h>
#include <ACAN.h>


class logType {
    public:
        uint8_t secs;
        uint8_t mins;
        uint8_t hours;
        uint8_t days;
        uint8_t months;
        uint16_t years;
        virtual const String toString() = 0;
};

class BBCANMessage : public logType {
    public:
        CANMessage msg;
        
        const String toString(){
            char tmp[200];
            snprintf(tmp,200,"\"%02d/%02d/%04d %02d-%02d-%02d\",%" PRIx32 ",%d,%d,%d,%" PRIu64 "",days,months,years,hours,mins,secs,msg.id,msg.ext,msg.rtr,msg.len,msg.data64);
            return String(tmp);
        }
        uint32_t toString(char* buf){
            snprintf(buf,200,"\"%02d/%02d/%04d %02d-%02d-%02d\",%" PRIx32 ",%d,%d,%d,%" PRIu64 "",days,months,years,hours,mins,secs,msg.id,msg.ext,msg.rtr,msg.len,msg.data64);
        }
        static const String getHeader(){
            return "\"time\",\"id\",\"ext\",\"rtr\",\"len\",\"data\"";
        }
};