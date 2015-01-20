#include "lpc21xx_keil.h"
#include "fastfunc.h"

#define LED1PIN  	12
#define LED2PIN  	13

FASTRUN void setLED1(int on)
{
	if (on) {
		IOCLR0 = (1<<LED1PIN);
	}
	else {
		IOSET0 = (1<<LED1PIN);
	}
}

FASTRUN void setLED2(int on)
{
	if (on) {
		IOCLR0 = (1<<LED2PIN);
	}
	else {
		IOSET0 = (1<<LED2PIN);
	}
}
