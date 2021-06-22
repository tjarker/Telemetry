/***********************************************************************************
 * @file    MutexLocker.h                                                          *
 * @author  Tjark Petersen                                                         *
 * @brief   This file supplies an environment WITH_MTX which executes the provided *
 *          code block with a lock and release of the provided mutex.              *
 ***********************************************************************************/ 

#ifndef __MUTEXLOCKER_H__
#define __MUTEXLOCKER_H__
#include <ChRt.h>

/**
 * @brief   ? 
 * @param   mtx   
 * @return  0 
*/
uint32_t mtxLockerPrologue(ch_mutex *mtx){
    chMtxLock(mtx);
    return 0;
}

/**
 * @brief Execute a code block with the provided mutex
*/
#define WITH_MTX(mtx) for (uint32_t i = mtxLockerPrologue(&mtx); i < 1; i++, chMtxUnlock(&mtx))

#endif