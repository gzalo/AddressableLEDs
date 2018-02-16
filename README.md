# AddressableLEDs
Driver for WS2812 addressable LEDs. Supports different light effects, and OSC to sync with music beats (planned)

## PCB folder
This folder contains a PCB for a simple addressable LED driver board (based on an ATmega8). It can be opened with the free version of Eagle.

## UC folder
This folder contains the microcontroller code that acts as a bridge between the LED strip and the computer. The microcontroller is connected to a PC using a cheap USB-TTL converter (it must be able to support 256000 bps, so a modern converter is needed, such as cp2102, pl2303, ft232rl).

## PC folder
This folder contains the C++ code that can be used to send patterns to the LEDs. It can be compiled under Linux or Windows, and sends data directly to a serial port. OSC control is planned, not yet implemented.