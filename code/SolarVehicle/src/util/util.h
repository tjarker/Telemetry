#ifndef __UTIL_H__
#define __UTIL_H__
#include <Arduino.h>
#include <ChRt.h>


class MutexLocker{
    private: bool killed = false;
    private: mutex_t *mtx;
    public: MutexLocker(mutex_t *mtx){
        this->mtx = mtx;
        chMtxLock(mtx);
    }
    public: ~MutexLocker(){
        chMtxUnlock(mtx);
    }
    public: void kill(){
        killed = true;
    }
    public: bool isKilled(){
        return killed;
    }
};

#define WITH_MTX(mtx) for(MutexLocker x(&mtx); !x.isKilled(); x.kill())

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