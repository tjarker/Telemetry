#ifndef __THREAD_STATE_H__
#define __THREAD_STATE_H__

#include <Arduino.h>
#include <ChRt.h>


class ThreadState {
  public:
    uint8_t isRunning = true;
    uint8_t terminate = false;
    uint8_t pause = false;
    thread_reference_t trp = NULL;

  public: void suspend(){
    chSysLock();
    chThdSuspendS(&trp);
    chSysUnlock();
  }
  public: void wakeUp(){ // msg is 16 bits
    chSysLock();
    chThdResumeI(&trp,(msg_t)0);
    pause = false;
    chSysUnlock();
  }
};

#endif