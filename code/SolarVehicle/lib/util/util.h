#ifndef __UTIL_H__
#define __UTIL_H__
#include <Arduino.h>

class ExecutionTimeMeasurer{
    bool killed;
    const char* name;
    uint32_t startTime;
    public:
        ExecutionTimeMeasurer(const char* name);
        ~ExecutionTimeMeasurer();
        void kill();
        bool isKilled();
};

#define MEASURE_EXEC_TIME(name) for (ExecutionTimeMeasurer x(name); !x.isKilled(); x.kill())

class ActivityMeasurer{
    bool killed;
    uint32_t startTime;
    public:
        ActivityMeasurer();
        ~ActivityMeasurer();
        void kill();
        bool isKilled();
};

#define SHOW_ACTIVITY for (ActivityMeasurer x; !x.isKilled(); x.kill())

class ActivityAndExecutionTimeMeasurer{
    bool killed;
    const char* name;
    uint32_t startTime;
    public:
        ActivityAndExecutionTimeMeasurer(const char* name);
        ~ActivityAndExecutionTimeMeasurer();
        void kill();
        bool isKilled();
};

#define ACT_PLUS_MEAS(name) for (ActivityAndExecutionTimeMeasurer x(name); !x.isKilled(); x.kill())

#endif