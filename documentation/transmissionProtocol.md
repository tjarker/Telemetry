<a name="top"></a>
[back to README](../README.md)

# Transmission Protocol

This document specifies the protocol used for sending data via RF between the two vehicles.

## Table of Contents
- [Transmission Protocol](#transmission-protocol)
  - [Table of Contents](#table-of-contents)
  - [Message layout](#message-layout)
  - [nRF24L01+](nRF24L01+)
---
[back to top](#top)
## nRF24L01+

The nRF24L01+ transceiver module uses a packet structure known as Enhanced ShockBurst. This simple packet structure is broken down into 5 different fields, which is illustrated below. 

<img src="https://github.com/tjarker/FagProjekt/blob/main/documentation/images/RFProtocol.png" alt="Enhanced ShockBurst Protocol">

[Read full article](https://lastminuteengineers.com/nrf24l01-arduino-wireless-communication/)

---
[back to top](#top)
## Message layout
