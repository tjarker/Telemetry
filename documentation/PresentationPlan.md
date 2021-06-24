# Presentation plan 

1. Introduction - Victor
2. Block diagram design - Steffan
3. CAN bus - Victor
4. Continue block diagram design - Steffan
5. RF communication - Steffan
6. Hardware implementation - Tjark
7. Software implementation - Tjark
8. RTOS - Tjark
9. Testing and Validation - Victor
10. Discussion throughput - Tjark
11. Discussion range - Steffan 
12. Conclusion - Victor 

## Tjark
- Implemented using arm-based microcontrollers of Teensy line-up
- All units in solar car are powered through 4-pin molex at 12V which also carries CAN bus singals
- In SCU 12V regulated to 5V and 3.3V internally
- The unit in support vehicle we chose to power through the serial connection which had to be established anyway
- On the software side, we used a real time operating system to implement our design
- a RTOS allows multiple threads to run concurrently on a processor by giving them execution slots
- This is taken care of by a small kernel running in the background which delegates time slots based on priorities associated with each thread
- The real time kernel also handles timing, such as putting a thread to sleep for a defined amount of time, or inter-thread communication
- In the SCU we use 4 threads to separate the systems functionality and in the SVU 2. The execution flow of each thread can be seen here in Fig. 4
- We implemented a prototype graphical user interface in the scala language
- Through the interface, commands can be sent over to the SCU, such as putting the SCU to sleep or enabling encryption

---

- As Victor said, our peak throughput capability is around 500 messages per second which fall short of the theoretical peak input stream of 2659 messages per second
- This peak throughput is only achieved if messages with no data payload are send which is quiet an urealistic premise
- The throughput could be improved by optimizing our software and the internal communication
- for instance many of the libraries we use hide away interrupts related to the library functionality and only give access to new data through polling
- Implementing a more interrupt based architecture would allow for finer grained management of the execution flow from our side

## Steffan
- "Our system is outlined by this schematic (points to system schematic)." 
- "The solar car unit interacts with the on-board CAN bus and streams it wirelessly to the support vehicle unit via an RF connection."
- "The RF transceivers provide two-way serial communication between the units over the 2.4 GHz ISM band."
- "Messages sent between the nodes are structures as Enhanced Shockburst packets complete with an address, control and flexible data payload fields."
- "RF messages can optionally be encrypted using an RSA algorithm."
- "From the support vehicle unit, the streamed CAN data is transmitted serially to a laptop in the support vehicle. A TelemetryUI window allows the user to view the incoming data and respond with commands or a CAN message."
- "In addition, the laptop hosts a UDP stream on the local network that can be accessed for instance in Matlab."

---

- "The maximum achieved transmission range fell way short of the 400-1000 m set as a goal for the system. This may be caused by a variety of factors influenced both by hardware and software." 
- "For instance, the RF module requires a very stable 3.3V supply which we were unfortinately unable to integrate on the perfboard."
- "On the software side, time constraints meant that we were not able to test all of the configurations of our RF module to ensure optimal long-range performance."
