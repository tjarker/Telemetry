#include <Arduino.h>
#include <ACAN.h>


class logType {
    public:
        uint8_t secs;
        uint8_t mins;
        uint8_t hours;
        virtual const String toString() = 0;
        virtual void reset() = 0;
};

class BBCANMessage : public logType {
    public:
        uint32_t id;
        bool ext;
        bool rtr;
        uint8_t len;
        uint64_t data;
        CANMessage msg;
        
        const String toString(){
            char tmp[200];
            snprintf(tmp,200,"\"%02d-%02d-%02d\", %8" PRIx32 ",     %1d,     %1d,     %1d,   %19" PRId64 "",hours,mins,secs,msg.id,msg.ext,msg.rtr,msg.len,msg.data64);
            return String(tmp);
        }
        static const String getHeader(){
            return "  \"time\"  ,   \"id\"  , \"ext\", \"rtr\", \"len\",         \"data\"";
        }
        void reset(){
            id = 0;
            ext = 0;
            rtr = 0;
            len = 0;
            data = 0LL;
        }
};