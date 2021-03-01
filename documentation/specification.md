[back to README](../README.md)

# Specifcations

## Questions
- How are the systems and connected parameters to be logged known?
- How large should the SD card be?
- How large should files be?
- How large should the buffer be?

---

## Solar Car Unit (SC)

|Feature ID|Description|
|:-|:-|
|**SC.SD**          |The system uses a SD card as a permanent storage of log files
|SC.SD.write        |Write a data structure to a file on the SD card
|SC.SD.read         |Read a file into a data structure
||
|**SC.RF**          |The system needs to communicate via a RF module with the support vehicle
|SC.RF.rxCmd        |Receive a system configuration command from the support vehicle
|SC.RF.rxCanCmd     |Receive a message to be broadcasted via the solar car CAN bus
|SC.RF.txResp       |Transmit response to a received command
|SC.RF.txLogUnit    |Transmit a log unit to the support vehicle
|SC.RF.txLogFile    |Transmit a log file to the support vehicle
||
|**SC.CAN**         |The system needs to communicate via the solar car CAN bus
|SC.CAN.rx          |Receive all messages broadcasted on the CAN bus
|SC.CAN.tx          |Transmit a message on the CAN bus
||
|**SC.BUF**         |The system needs to buffer logged data in local flash memory until enough data to fill a file is collected
|SC.BUF.add         |Add a log unit to the buffer
|SC.BUF.rd          |Read a logging unit from the buffer into RAM
|SC.BUF.rdToFile    |Read all logged data from the buffer into a file
|SC.BUF.clr         |Empty the buffer

## Support Vehicle Unit (SV)

|Feature ID|Description|
|:-|:-|
|**SV.RF**          |The system needs to communicate via RF with the solar car
|SV.RF.txCmd        |Transmit a system configuration command to the solar car
|SV.RF.txCanCmd     |Transmit a CAN message to the solar car
|SV.RF.rxResp       |Receive a response from the solar car unit
|SV.RF.rxLogUnit    |Receive a log unit from the solar car unit
|SV.RF.rxLogFile    |Receive a log file from the solar car unit
||
|**SV.UART**        |The system needs to communicate with a computer via UART
|SV.UART.rxCmd      |Receive a command via UART
|SV.UART.rxCanCmd   |Receive a CAN bus message via UART
|SV.UART.txLogUnit  |Transmit a log unit to the computer
|SV.UART.txLogFile  |Transmit a log file to the computer

## Support Vehicle Computer Program (PR)

|Feature ID|Description|
|:-|:-|
|**PR.UART**        |The program needs to communicate with the support vehicle unit via UART
|PR.UART.txCmd      |Transmit a system configuration command via UART
|PR.UART.txCanCmd   |Transmit a CAN message via UART
|PR.UART.rxLogUnit  |Receive a log unit via UART
|PR.UART.rxLogFile  |Receive a log file via UART
||
|**PR.FILE**        |The program needs to output received log files
|PR.FILE.write      |Write a file containing the last received log file
||
|**PR.GUI**         |The program is interface via a GUI
|PR.GUI.getConf     |Get configuration parameters from GUI
|PR.GUI.getCanCmd   |Get a CAN message from the GUI
|PR.GUI.monitor     |Show incoming log data stream
|PR.GUI.saveLog     |Pass log file to file writer
