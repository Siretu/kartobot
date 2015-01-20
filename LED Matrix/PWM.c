/*
 * PWM.c
 *
 *  Created on: 21 Jul 2011
 *      Author: exuvo
 */

#include <avr/io.h>
#include "PWM.h"

//void TimersInit() is in time.c

/* timers 0,1,2 are used for phase-correct hardware PWM
 * this is better for motors as it ensures an even waveform
 * Only timer 1 supports 16-bit PWM, 0,2 are 8-bit
 * Remember to set pin to Output before use.
 */
void PWM(uint8_t pin, uint8_t val){
	/**
	 * If PWM is in fast mode val == 0 is fail, fix:
	 * if (val == 0) {
	 *   PORT &= ~_BV(PIN);
	 * } else { PWM() }
	 */
	
	if (pin == TIMER1A) {
		TCCR1A |= _BV(COM1A1);
		// set PWM duty
		OCR1A = val;
	} else if (pin == TIMER1B) {
		TCCR1A |= _BV(COM1B1);
		// set PWM duty
		OCR1B = val;
	} else if (pin == TIMER2) {
		TCCR2 |= _BV(COM21);
		// set PWM duty
		OCR2 = val;
	} else {
		extern void error(uint8_t errorCode, char* message);
		error(12, 0);
	}
}
