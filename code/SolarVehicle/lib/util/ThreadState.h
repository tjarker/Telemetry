#ifndef __THREAD_STATE_H__
#define __THREAD_STATE_H__

#include <Arduino.h>
#include <ChRt.h>

/**
 * Class for interthread communication.
 * 
 * The class contains state fields and methods for going into suspended mode from inside a thread
 * and waking the thread from another thread
 * 
 * The suspend method should only be called when the pause field is asserted.
 * 
 */
class ThreadState {
  public:
    uint8_t isRunning = true; 
    uint8_t terminate = false;
    uint8_t pause = false;
    thread_reference_t trp = NULL;

  /**
   * Puts the current thread into suspend mode, halting its execution. Should only be called if pause is asserted
   */
  public: void suspend(){
    chSysLock();
    chThdSuspendS(&trp);
    chSysUnlock();
  }

  /**
   * Wakes up the thread which was put into suspend mode by the call to suspend()
   */
  public: void wakeUp(){ // msg is 16 bits
    chSysLock();
    chThdResumeI(&trp,(msg_t)0);
    pause = false;
    chSysUnlock();
  }
};

#endif