#ifndef __MUTEXLOCKER_H__
#define __MUTEXLOCKER_H__

#include <ChRt.h>

/**
 * This file supplies an environment WITH_MTX which executes the provided
 * code block with a lock and release of the provided mutex.
 */

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

/**
 * Execute a code block with the provided mutex
 */
#define WITH_MTX(mtx) for(MutexLocker x(&mtx); !x.isKilled(); x.kill())

#endif