[back to README](../README.md)
# Components

## Main Controller

### Teensy
- Homepage: https://www.pjrc.com/teensy/

### ARTIX-7 FPGA:
- RS: https://dk.rs-online.com/web/p/fpga-udvikling/1346483/
- with 256MB SDRAM: https://dk.rs-online.com/web/p/fpga-udvikling/1346478/

### CAN:
- Arduino shield: https://store.arduino.cc/can-bus-shield-v2
- Pmod: https://dk.rs-online.com/web/p/kommunikation-og-tradlos-udvikling/1720815/

### Patmos
- Patmos soft core: https://github.com/t-crest/patmos
- Patmos reference handbook: http://patmos.compute.dtu.dk/patmos_handbook.pdf

---

## External Memory
### SST26VF032B: 
* https://docs.rs-online.com/8ccd/0900766b813d5922.pdf
### W25Q128FVSG: 
* https://www.pjrc.com/teensy/W25Q128FV.pdf
### PMod:
* https://dk.rs-online.com/web/p/memory-udvikling/1840475/
---

## RF transceiver modules
### CC1101: 
*-* https://www.ti.com/lit/ds/symlink/cc1101.pdf?ts=1612980410973&ref_url=https%253A%252F%252Fwww.google.com%252F
### nRF24L01+PA+LNA: 
* https://www.sparkfun.com/datasheets/Components/SMD/nRF24L01Pluss_Preliminary_Product_Specification_v1_0.pdf
* https://www.amazon.com/MakerFocus-NRF24L01-Transceiver-Antistatic-Compatible/dp/B01IK78PQA/ref=sr_1_2_sspa?dchild=1&keywords=NRF24L01%2BPA%2BLNA+Wireless+Transceiver&qid=1614330857&sr=8-2-spons&psc=1&spLa=ZW5jcnlwdGVkUXVhbGlmaWVyPUExUFNBTTRTU0tCNEtKJmVuY3J5cHRlZElkPUEwMTY1ODczTVRWMFM5WENNVURXJmVuY3J5cHRlZEFkSWQ9QTA0NDY4MjExSVZBNFo4UVJEOURHJndpZGdldE5hbWU9c3BfYXRmJmFjdGlvbj1jbGlja1JlZGlyZWN0JmRvTm90TG9nQ2xpY2s9dHJ1ZQ==
### nRF905: 
* https://infocenter.nordicsemi.com/pdf/nRF905_PS_v1.5.pdf
### SX1278 (LoRa):
* https://www.semtech.com/products/wireless-rf/lora-transceivers/sx1278
* https://www.amazon.com/Gowoops-Wireless-Transmitter-Compatible-Microcomputer/dp/B07G7ZRLQB/ref=sr_1_2?dchild=1&keywords=SX1278&qid=1613047504&sr=8-2
### HC-12: 
* https://opencircuit.shop/resources/file/0f8d974f31fd813604c4d3fb0e9004ec3b483706466/HC-12-Datasheet.pdf

### Comparison
| Model | Range (m) | Frequency (Hz) | RS |
|:-----:|:---------:|:--------------:|:--:|
| **[CC1101](https://www.ti.com/lit/ds/symlink/cc1101.pdf?ts=1612980410973&ref_url=https%253A%252F%252Fwww.google.com%252F)** | | 315/433/868/915 MHz | [link](https://dk.rs-online.com/web/p/rf-ic-er/9212971/) |
| **[HC-12](https://opencircuit.shop/resources/file/0f8d974f31fd813604c4d3fb0e9004ec3b483706466/HC-12-Datasheet.pdf)** | 1000 m | 433.4-473.0 MHz | [link]() | 
| **[nRF24](https://www.sparkfun.com/datasheets/Components/SMD/nRF24L01Pluss_Preliminary_Product_Specification_v1_0.pdf)** | 1000 m | 2.4 GHz | [link](https://dk.rs-online.com/web/p/rf-ic-er/8855748/) |
| **[nRF905](https://infocenter.nordicsemi.com/pdf/nRF905_PS_v1.5.pdf)** | | 433/868/915 MHz | [link](https://dk.rs-online.com/web/p/rf-ic-er/8855794/) |
| **[SX1278 (LoRa)](https://semtech.my.salesforce.com/sfc/p/#E0000000JelG/a/2R0000001Rc1/QnUuV9TviODKUgt_rpBlPz.EZA_PNK7Rpi8HA5..Sbo)** | | UHF | [link]() |

## CAN-bus tranceiver modules
### SN65HVD232D
* https://www.ti.com/lit/ds/symlink/sn65hvd230.pdf?ts=1614315589062&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FSN65HVD230
* [RS](https://dk.rs-online.com/web/p/can-interface-ic-er/4619771/)
### PCA82C251T/YM,112
* [RS](https://dk.rs-online.com/web/p/can-transceivere/7974709/)
