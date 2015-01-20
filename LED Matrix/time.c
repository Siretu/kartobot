/*
 * time.c
 *
 *  Created on: 21 Jul 2011
 *      Author: exuvo
 */

#include "time.h"
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "sleep.h"

// the prescaler is set so that the timer ticks every 64 clock cycles, and the
// the overflow handler is called every 16320 ticks.
//816
#define MICROSECONDS_PER_TIMER_OVERFLOW (clockCyclesToMicroseconds(64 * 0xFF))

// the whole number of milliseconds per timer0 overflow
//0.816
#define MILLIS_INC (MICROSECONDS_PER_TIMER_OVERFLOW / 1000)

// the fractional number of milliseconds per timer0 overflow
#define FRACT_INC (MICROSECONDS_PER_TIMER_OVERFLOW % 1000)
#define FRACT_MAX (1000)

volatile uint32_t timer_overflow_count = 0;
volatile uint32_t timer_millis = 0;
static uint16_t timer_fract = 0;

ISR(TIMER0_OVF_vect){
	// copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)
	uint32_t m = timer_millis;
	uint16_t f = timer_fract;

	m += MILLIS_INC;
	f += FRACT_INC;
	if (f >= FRACT_MAX) {
		f -= FRACT_MAX;
		m += 1;
	}

	timer_fract = f;
	timer_millis = m;
	timer_overflow_count++;

//	extern volatile uint8_t tired;
//	if(tired){
//		extern volatile uint32_t beganSleeping;
//		extern volatile uint8_t sleepTransition;
//
//		if(usbConnected()){//Computer has appeared
//			awaken();
//
//		}else if(m - beganSleeping > 300000){//5min
//			sleepTransition = 1;//Go from ExtendedStandby to Power-down
//			awaken();
//		}
//	}
}

uint32_t millis(){
	uint32_t m;
	uint8_t oldSREG = SREG;

	// disable interrupts while we read timer0_millis or we might get an
	// inconsistent value (e.g. in the middle of a write to timer0_millis)
	cli();
	m = timer_millis;
	SREG = oldSREG;

	return m;
}

uint32_t micros() {
	uint32_t m;
	uint8_t oldSREG = SREG, t;

	cli();
	m = timer_overflow_count;
	t = TCNT0;

	if ((TIFR & _BV(TOV0)) && (t < 255)){
		m++;
	}

	SREG = oldSREG;
	return ((m << 8) + t) * (32 / clockCyclesPerMicrosecond());
}

void delay(uint32_t ms){
	uint32_t start = millis();

	while (millis() - start <= ms){

	}
}

void TimersInit(){
	//PWM for ECS desire pulse widths of 1ms

	//timer 0 - prescale factor 64 - ?us
	TCCR0 |= _BV(CS01) | _BV(CS00);

	//timer 0 overflow interrupt
	TIMSK |= _BV(TOIE0);

	//timer 1 - prescale factor 64 - ?us max pulse width
	TCCR1B |= _BV(CS11) | _BV(CS10);

	//timer 1 - 8-bit phase correct PWM
	TCCR1A |= _BV(WGM10);

	//timer 2 - prescale factor 64 - ?us
	TCCR2 |= _BV(CS22);

	//timer 2 - 8-bit phase correct pwm
	TCCR2 |= _BV(WGM20);
}


