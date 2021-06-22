/***********************************************************************************************************
 * @file    ThreadState.h                                                                                  *
 * @author  Tjark Petersen                                                                                 *
 * @brief   Class for interthread communication.                                                           *
 * @note    The class contains state fields and methods for going into suspended mode from inside a thread *
 *          and waking the thread from another thread.                                                     *
 *          The suspend method should only be called when the pause field is asserted.                     *
 ***********************************************************************************************************/

#ifndef __THREAD_STATE_H__
#define __THREAD_STATE_H__
#include <Arduino.h>
#include <ChRt.h>

/**
 * @brief   Class to represent current state of thread. 
 */ 
class ThreadState 
{
  /* <!-- Public member variables --> */
  public:
    uint8_t isRunning = true;       //
    uint8_t terminate = false;      //
    uint8_t pause = false;          // 
    thread_reference_t trp = NULL;  // ? 

  /* <!-- Public member functions --> */
  /**
   * @brief   Puts the current thread into suspend mode, halting its execution. Should only be called if pause is asserted. 
   * @return  void
   */
  public: void suspend(){
    chSysLock();                    // Lock real-time kernel
    chThdSuspendS(&trp);            // Suspend active thread
    chSysUnlock();                  // Unlock real-time kernel
  }

  /**
   * @brief   Wakes up the thread which was put into suspend mode by the call to suspend(). 
   * @return  void
   */
  public: void wakeUp()
  { 
    chSysLock();                    // Lock real-time kernel
    chThdResumeI(&trp, (msg_t)0);   // msg is 16 bits
    pause = false;  
    chSysUnlock();                  // Unlock real-time kernel
  }
};

#endif