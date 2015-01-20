/*
 * ADC.c
 *
 *  Created on: 21 Jul 2011
 *      Author: exuvo
 */

#include <avr/io.h>
#include "ADC.h"

uint8_t analog_reference = DEFAULT;

void ADCinit(){
	// 20 MHz / 128 = 156 KHz, inside the desired 50-200 KHz range.
	ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

	// enable a2d conversions
	ADCSRA |= _BV(ADEN);
}

uint16_t ADCread(uint8_t pin){
	uint8_t low, high;

	// set the analog reference (high two bits of ADMUX) and select the
	// channel (low 4 bits).  this also sets ADLAR (left-adjust result)
	// to 0 (the default).
	ADMUX = (analog_reference << 6) | (pin & 0x07);
  
	// without a delay, we seem to read from the wrong channel
	//delay(1);

	// start the conversion
	ADCSRA |= _BV(ADSC);

	// ADSC is cleared when the conversion finishes
	while (bit_is_set(ADCSRA, ADSC));

	// we have to read ADCL first; doing so locks both ADCL
	// and ADCH until ADCH is read.  reading ADCL second would
	// cause the results of each conversion to be discarded,
	// as ADCL and ADCH would be locked when it completed.
	low = ADCL;
	high = ADCH;

	return (high << 8) | low;
}
