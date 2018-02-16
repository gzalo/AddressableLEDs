F_CPU = 16000000
DEVICE = atmega8
CFLAGS = -Wall -Os -mmcu=$(DEVICE) -DF_CPU=$(F_CPU) 

all:
	avr-gcc $(CFLAGS) -c light.c -o light.o
	avr-gcc $(CFLAGS) -c light_ws2812.c -o light_ws2812.o
	avr-gcc $(CFLAGS) -o light.elf light.o light_ws2812.o
	
	avr-objcopy -j .text -j .data -O ihex light.elf light.hex
