/*
 * utils.c
 *
 *  Created on: 15 Mar 2012
 *      Author: exuvo
 */

#include "utils.h"
#include <util/delay.h>
#include "bar.h"
#include <avr/interrupt.h>

//http://bildr.org/2011/02/74hc595/
void shift(volatile uint8_t *port, uint8_t SCK_Pin, uint8_t RCK_Pin, uint8_t SER_Pin, uint32_t data, uint8_t bits){
	*port &= ~_BV(RCK_Pin); 				// Set the register-clock pin low

	for(uint8_t i=0; i < bits; i++){
		*port &= ~_BV(SCK_Pin); 				//Set the serial-clock pin low
		*port |= ((data>>i)&0x1) << SER_Pin; 	//Go through each bit of data and output it
		*port |= _BV(SCK_Pin); 					//Set the serial-clock pin high
		*port &= ~_BV(SER_Pin);					//Set the data pin low again
	}

	*port |= _BV(RCK_Pin);				// Set the register-clock pin high to update the output of the shift-register
}

uint16_t reverse(uint16_t x){
    x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
    x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
    x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
    return((x >> 8) | (x << 8));
}

/*
 * 12 = not a PWM pin
 */
void error(uint8_t errorCode){
	cli();
	BarUpdate(errorCode);
	//TODO print E on matrix

	while(1){
//		matrixDraw();
		_delay_ms(2);
	}
}

/*
 * 3-9 = local i2c errors
 * 10 = hardware serial buffer full
 * 11 = SimpleTimer full
 */
void warn(uint8_t errorCode){
//	uint8_t d[] = {'w', errorCode};
//	twi_writeTo(QUAD2, d, sizeof(d), false);//Probably not work at all
}

/* This function gets the current value of the heap and stack pointers.
* The stack pointer starts at the top of RAM and grows downwards. The heap pointer
* starts just above the static variables etc. and grows upwards. SP should always
* be larger than HP or you'll be in big trouble! The smaller the gap, the more
* careful you need to be. Julian Gall 6 - Feb - 2009.
*/
uint16_t freeRAM(){
	uint8_t * heapptr, * stackptr;
	stackptr = (uint8_t *)malloc(4); // use stackptr temporarily
	heapptr = stackptr; // save value of heap pointer
	free(stackptr); // free up the memory again (sets stackptr to 0)
	stackptr = (uint8_t *)(SP); // save value of stack pointer
	return stackptr - heapptr;
}
