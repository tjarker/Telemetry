#include "command.h"
#include "BlackBox.h"
#include <ACAN.h>

BlackBox box;
CANMessage frame;

/**
 * @brief   Sets Command values to default for normal operation
*/
void Command::Default(){
    Command::log = true;
    Command::reset = false;
    Command::filter_id = 0x00;
    Command::mask = 0x00;
}

/**     Hvis man ikke ønsker at logge i et tidsrum, vil man så blot starte en ny fil når man vil logge igen?
 * @brief   Toggles whether the blackbox should be logging info or not
*/
void Command::Log_toggle(){

}

/**
 * @brief   Ends current log and starts a new log. Also sets the
 *  command value "reset" to false to avoid continuous reset, if
 *  not ordered to do so
*/
void Command::Reset_log(){
    box.endLogFile();
    box.startNewLogFile();
    Command::reset = false;
}

/**
 * @brief   Sets a filter for to send and receive data with a 
 *  specific ID. Using the mask is optional, and should be set
 *  to 1F if not used. kRemote and kStandard are library
 *  defined constants.
 * 
 * ISSUE:   Find ud af om hele CAN skal startes forfra for at
 *  bruge et filter, i så fald med syntaks,
 *  ACAN::can1.begin (settings, filter, 1);. Står normalt i
 *  setup().
*/
void Command::Filter(uint8_t filter_id, uint8_t mask){
    //--- ACAN::can1 receives remote frames
    ACANPrimaryFilter filter [1] = {
        ACANPrimaryFilter (kRemote, kStandard, mask, filter_id)   
    } ;
    
    //--- ACAN::can0 sends remote frames - Dette er vel allerede implementeret?
    //frame.id = filter_id;
    //frame.rtr = true;
}