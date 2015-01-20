/*
 * bar.c
 *
 *  Created on: 15 Mar 2012
 *      Author: exuvo
 */

#include <inttypes.h>
#include <avr/io.h>
#include "bar.h"
#include "utils.h"

void BarUpdate(uint16_t value){
	value = reverse(~value)>>6;
	shift(&PORTC, 2, 1, 0, value & 0xFF, 8);

	PORTB &= ~(_BV(1) | _BV(2));

	if(value & _BV(8)){
		PORTB |= _BV(2);
	}

	if(value & _BV(9)){
		PORTB |= _BV(1);
	}
}
