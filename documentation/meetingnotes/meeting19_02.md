[back to README](../../README.md)
# Meeting Friday 19.02.2021

## Where: Online

## Participants:
 - Tjark
 - Steffan
 - Victor
 - Martin Schoeberl

## Agenda:
 - Presentation of the project
 - Discussion of implementation possibilities
   - Implementation on FPGA, digital design with VHDL/Verilog/Chisel
   - Soft core on FPGA?
   - Flash Memory (SPI?, file system?)
   - Interface with RF module and CAN bus  
   
---

### Presentation of the project
 - two modules:
   - one in the solar car
   - one in the support vehicle
 - wireless link up to 1 km between the modules
 - data stream of up to 250kbits needs to be supported
 - Solar car module:
   - receives CAN bus messages as input (CAN SPI module?)
   - saves a message log locally (black box)
   - directly sends messages to the support vehicle unit
   - can receive CAN messages from support vehicle and broadcast them on the solar car CAN bus
   - (can receive system configuration commands from support vehicle)?
 - Support vehicle module:
   - receives CAN messages from solar car module
   - outputs CAN messages via USB
   - receives CAN messages via USB 
   - sends CAN messages wirelessly to solar car module
   - (sends system configuration received via USB wirelessly to solar car module)?
   
 - what hardware are we building upon?
 - what is the software stack available on that platform?
 - create more concrete implementation proposals
 - Martin asks about sd card interface for patmos
 - patmos: https://github.com/t-crest/patmos
 - hardware or software?
   - software is easier to implement, more flexible
   - MCU implementation is the easiest, benefits from software improvements
   - hardware is more challenging (maybe more fun? :)).
   - We can explore both solutions, or a combination, before making a final decision
