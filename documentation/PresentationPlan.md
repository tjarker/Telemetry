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
- In the SCU we use 4 threads to implement the systems functionality and in the SVU 2. The execution flow of each thread can be seen here in Fig. 4

---

- As Victor said, our peak throughput capabilities fall short of the theoretical input stream of peak 2659 messages per second
- This peak throughput is only achieved if messages with no data payload are send which is quiet an urealistic premise
- The throughput could be improved by optimizing our software and the internal communication
- for instance many of the libraries we use hide away interrupts related to the library functionality and only give access to new data through polling
- Implementing a more interrupt based architecture would allow for finer grained management of the execution flow from our side
