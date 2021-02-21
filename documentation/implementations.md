[Back to README](../README.md)

# Implementation possibilities

![System Schematic](images/schematic.png)

---

# Solar Car unit
<table>
<tr><th>Main Controller </th><th> CAN Bus Controller</th><th>External Memory</th></tr>
<tr><td>

[**Teensy**](components.md#teensy):
- pros:
    - access to Arduino software stack
- cons:
---
[**Patmos**](components.md#patmos):
- pros:
- cons:

</td><td>

Use the available CAN bus controller (SPI?)
- pros:
- cons:

---

Design own CAN bus controller on FPGA
- pros:
- cons:
    - Would require custom circuitry in front of FPGA

</td><td>

1. [SPI SDCard](components.md#externalmemory)
    - SPI interface will most likely be needed anyways for RF module

</td></tr> </table>

---

# Support Vehicle unit
<table>
<tr><th>Controller </th><th> UART Controller </th></tr>
<tr><td>

1. Arduino
2. [Teensy](components.md#teensy)

</td><td>

1. Integrated in uC
2. Implement on FPGA

</td></tr> </table>

---

# Support Vehicle Laptop

<table>
<tr><th>Interface </th></tr>
<tr><td>

1. Mini GUI to send and receive
2. More advanced GUI with a small monitoring dashboard
2. Just send data via UART
3. Command line interface

</td></tr> </table>

---

# Wireless Transmission
<table>
<tr><th>RF Transceiver Modules </th></tr>
<tr><td>

1. [CC1101](components.md#CC1101)
2. [nRF24](components.md#nrf24)
3. [nRF905](components.md#nrf905)
4. [SX1278 (LoRa)](components.md#sx1278)
    - long range and low power
5. [HC12](components.md#hc12)

</td></tr> </table>