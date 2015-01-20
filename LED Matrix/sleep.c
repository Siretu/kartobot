/*
 * power.c
 *
 *  Created on: 21 Jul 2011
 *      Author: exuvo
 */

#include <inttypes.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include "time.h"

volatile uint8_t tired;
volatile uint32_t beganSleeping;
volatile uint8_t sleepTransition;

static void ADCon(){
	ADCSRA |= _BV(ADEN);
}

static void ADCoff(){
	ADCSRA &= ~_BV(ADEN);
}

static void USARTon(){
	UCSRB |= _BV(TXEN);
	UCSRB |= _BV(RXEN);
}

static void USARToff(){
	UCSRB &= ~_BV(RXEN);
	UCSRB &= ~_BV(TXEN);
}

static void TWIon(){
	TWCR = _BV(TWEN);
}

static void TWIoff(){
	TWCR &= ~_BV(TWEN);
}

static void SPIon(){
	SPCR = _BV(SPE);
}

static void SPIoff(){
	SPCR &= ~_BV(SPE);
}

static void TimersOn(){
	TimersInit();
}

static void TimersOff(){
	TCCR0 = 0;
	TCCR1B = 0;
	TCCR2 = 0;
}

static void sleep(uint8_t mode){
	beganSleeping = millis();
	while(tired){
		MCUCR = mode | _BV(SE);
		sei();
		__asm__ __volatile__ ("sleep");
		cli();
	}
	MCUCR &= ~_BV(SE);
}

//Stop continually going to sleep
void awaken(){// "my child, and embrace the glory that is your birthright." - The Overmind
	tired = 0;
}

//Continually go to sleep after wake ups.
void sleepy(){
	tired = 1;
}

/* CPU off
 * Resume: 4 clock cycles.
 */
void SleepIdle(){
	cli();
	awaken();
	sleep(SLEEP_MODE_IDLE);
	sei();
}

/* Resume: 6 clock cycles.
 */
void SleepStandby(){
	cli();
//	ADCoff();
//	SPIoff();
//	USARToff();
	sleepy();
	sleep(SLEEP_MODE_STANDBY);
	if(sleepTransition){
		sleepTransition = 0;
		void SleepPowerDown();
		SleepPowerDown();
		return;
	}
//	ADCon();
//	SPIon();
//	USARTon();
	sei();
}

/* Everything off, except WDT for crude time keeping.
 * Resume: Reset Time-out period (16K CK) ~130ms + 4 clock cycles.
 */
void SleepPowerDown(){
	cli();
//	ADCoff();
//	SPIoff();
//	TWIoff();
//	USARToff();
	sleepy();
	TimersOff();
	sleep(SLEEP_MODE_PWR_DOWN);
	TimersOn();
//	ADCon();
//	SPIon();
//	TWIon();
//	USARTon();
	sei();
}

