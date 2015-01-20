//////////////////////////////////////////////////////////////////////////////
//
// Philips LPC2129 ARM7TDMI LED/Switch Example/"RAM-Function"
//
// This example demonstrates writing to and reading from 
// the GPIO port.
// (1) flash the LED 10 times
// (2) wait for key-press, turn off LED if key is pressed
// (3) using a function "in RAM"
//
// a WinARM example by Martin THOMAS, Kaiserslautern, Germany
// (eversmith@heizung-thomas.de)
// http://www.siwawi.arubi.uni-kl.de/avr_projects
//////////////////////////////////////////////////////////////////////////////

/*
The functions setLED1 and setLED2 are functions which is executed 
"from RAM". Both are located in section ".fastrun" (see fastfunc.h).
The contents of this section gets copied from ROM/FLASH to RAM
together with the "initialized data" (see .data and .fastram in
linker-script). To use the attribute long_call the functions have to
be located in a separate object-file which is fastfunc.o in this
example.

This code has been tested with arm-elf-gcc 4.0.0 (WinARM 4/05).
*/

#include "lpc21xx_keil.h"
#include "fastfunc.h"

#include <stdlib.h>

// olimex LPC-P2129: buttons on P0.10/P0.11 (active low)
#define BUT1PIN 	10
#define BUT2PIN 	11
// olimex LPC-P2129: LEDs on P0.12/P0.13 (active low)
#define LED1PIN  	12
#define LED2PIN  	13

static void delay(void )
{
	volatile int i,j;

	for (i=0;i<100;i++)
		for (j=0;j<1000;j++);
}
  
int main(void)
{
	int i;
	
	MAMCR = 2;	// MAM functions fully enabled

	IODIR0 |= (1<<LED1PIN)|(1<<LED2PIN); // define LED-Pins as outputs
	IOSET0 = (1<<LED1PIN)|(1<<LED2PIN); // set Bits = LEDs off (active low)
	IODIR0 &= ~((1<<BUT1PIN)|(1<<BUT2PIN));// define Button-Pins as inputs
		
	i=0;
	while (i<10)	
	{
		IOCLR0 = (1<<LED1PIN);	
		IOSET0 = (1<<LED2PIN);	
		delay();
		IOSET0 = (1<<LED1PIN);
		IOCLR0 = (1<<LED2PIN);	
		delay();
		i++;
	}
	
	while (1)	
	{
		if (IOPIN0 & (1<<BUT1PIN))	{ // true if button released (active low)
			setLED1(1); // call the "RAM-function" -> LED on 
		}
		else {
			setLED1(0); // call the "RAM-function" -> LED off
		}

		if (IOPIN0 & (1<<BUT2PIN))	{ // true if button released (active low)
			setLED2(1);
		}
		else {
			setLED2(0);
		}

	}
	
	return 0; // never reached
}
