# Throughput Analysis

## Data throughput

The theoretical maximum data throughput of the system is dictated by the CAN bus. For now (April 2021) the CAN bus of the solar car is operated at 125 kbps with 11 bit identifiers, but an upgrade to 250 kbps in the future is an option. Therefore the throughput analysis is conducted with a 250 kbps CAN bus implementation in mind.

One CAN frame consists of 44 bits + 0-8 data bytes ([wikipedia](https://en.wikipedia.org/wiki/CAN_bus#Frames)). In between frames a 3 bit break is inserted.

$\text{max}(f_{frame})=250\times10^3\;\frac{bit}{s}\div 47\frac{bit}{frame}\approx 5320\frac{frame}{s}$

$\text{min}(T_{frame})=\frac{1}{\text{max}(f_{frame})}\approx187\frac{\mu s}{frame}$

The CAN library used to communicate with the CAN controller in the Teensy 3.6 utilizes an ISR to service the incoming frame. The new frame can be read by polling a library function until a success message is returned. The reading of new message can be measured to take $???\mu s$ with a resolution of $1\mu s$.

The incoming frame is read into a custom data structure which consists of 19 bytes. This operation can be measured to take $???\mu s$ with a resolution of $1\mu s$.

The custom data structure is then written into the buffer of the black box. If the buffer is full it is written out to the SD card. This takes $???\mu s$ with a resolution of $1\mu s$. If the buffer is not full it only takes around $???\mu s$ with a resolution of $1\mu s$.

The custom data structure is also send out via radio to the support vehicle. Pushing the message to the RF transceiver can be measured to take $???\mu s$ with a resolution of $1\mu s$.

After this operation the system is ready to receive the next CAN frame. In total $???\mu s$ have been spend leading to a potential processing throughput of $???\frac{frames}{s}$. This is ??? than the CAN bus can feed and the system has on average $???\mu s$ idle time where system configuration and other side tasks can be conducted.

