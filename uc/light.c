/*

	Addressable LEDs, microcontroller code
	
	Receives data via the serial port (at 256 kbps), buffers it and then sends the values to the strip. 
	Edit ws2812_config.h if you want to change the data pin of the strip.
	
	Data format:
	
	0xFF 		   Delimiter
	0xRR 0xGG 0xBB First LED color
	0xRR 0xGG 0xBB Second LED color 
	0xRR 0xGG 0xBB Third LED color 
	....
	0xRR 0xGG 0xBB LED_COUNTth LED color
	
	0xFF  		   Delimiter
	0xRR 0xGG 0xBB First LED color
	... (the cycle restarts)
	
	
	To avoid syncing to the wrong part of the data, the 0xFF character 
	should only be used for the delimiter. This limits the maximum intensity of the LEDs to 0xFEFEFE, but its not really noticeable.
*/

#define F_CPU 16000000

/*
	Approx TIMING:
		4.4 ms update LEDs
		14.10 ms reception (at 256 kbps)
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>

#include "light_ws2812.h"

//Update this line to the length of the LED strip used
#define LED_COUNT 120 
struct cRGB colors[LED_COUNT];

//I/O pin initialization
void boardInit(){
	DDRB = 0b00000001;
	DDRD = 0b00000000;
	DDRC = 0b00000000;	
}

void toggleOnBoardLed(){
	PORTB ^= 1; 
}
void setOnBoardLed(uint8_t val){
	if(val == 1){
		PORTB |= 0x01; 
	}else{
		PORTB &= 0xFE; 
	}
}

#define BAUD 256000
#define BAUDRATE ((F_CPU+BAUD*8UL)/(BAUD*16UL)-1UL)

void uartInit(){
	UBRRH = (uint8_t)(BAUDRATE>>8);
	UBRRL = (uint8_t)(BAUDRATE);

	UCSRB = (1<<RXEN)|(1<<TXEN);
	UCSRC = (1<<URSEL)|(3<<UCSZ0);	
}
uint8_t getchar(){
	while(!(UCSRA & (1<<RXC)));
	return UDR;
}

int main(){
	uint8_t i;
	
	boardInit();
	uartInit();
	
	_delay_ms(100);
	setOnBoardLed(1);
	_delay_ms(500);
	setOnBoardLed(0);
			
	while(1){

		while(getchar()!=0xFF); //Wait for delimiter
		toggleOnBoardLed();
		
		for(i=0;i<LED_COUNT;i++){
			colors[i].r = getchar();
			colors[i].g = getchar();
			colors[i].b = getchar();
		}
		
		ws2812_setleds(colors, LED_COUNT);
	}
}
