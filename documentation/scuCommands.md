<a name="top"></a>
[back to README](../README.md)

# SCU Commands

This document specifies the commands the SCU can receive via RF.

## Table of Contents
- [SCU Commands](#scu-commands)
  - [Table of Contents](#table-of-contents)
  - [Configuration](#configuration)
  - [CAN messages](#can-messages)

---
[back to top](#top)
## Configuration

In this section system configuration commands are specified.

|Command Name|Command Code|Description|
|:-|:-:|:-|
|logOn      |0x????????     |Start logging CAN bus chatter
|logOff     |0x????????     |Stop logging CAN bus chatter

---
[back to top](#top)
## CAN messages

In this section it is specified how to frame a CAN message which is sent via RF and then broadcasted on the CAN bus in the SCU.