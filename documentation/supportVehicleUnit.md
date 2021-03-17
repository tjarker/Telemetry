<a name="top"></a>
[back to README](../README.md)

# The Support Vehicle Unit 

## Table of Contents

- [The Support Vehicle Unit](#the-support-vehicle-unit)
  - [Table of Contents](#table-of-contents)
  - [Specification](#specification)
    - [Interface](#interface)
    - [Features](#features)

---
[back to top](#top)
## Specification

### Interface

The following table gives an overview of the interface of the solar car module with its surroundings.

|Port Name|Direction|Description|
|:-|:-:|:-|
|RF|Bidirectional|Wireless communication channel to the solar car unit|
|UART|Bidirectional|Interface to a computer used for further data processing and system control via UART|

### Features

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
