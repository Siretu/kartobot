/*
 * main.cpp
 *
 *  Created on: 19 Jul 2011
 *      Author: exuvo
 */
#include "main.h"
#include <Print.cpp>
#include <PString/PString.cpp>
#include <SimpleTimer/SimpleTimer.cpp>
#include <Wire/utility/twi.h>
#include <Wire/Wire.cpp>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "EEPROM.h"
#include "sleep.h"
#include "twi.h"
#include "bar.h"
#include "matrix.h"

void * operator new(size_t size){
    return malloc(size);
}
void * operator new[](size_t size){
    return malloc(size);
}

void operator delete(void * ptr){
    free(ptr);
}
void operator delete[](void * ptr){
	free(ptr);
}

int __cxa_guard_acquire(__guard *g){
	return !*(char *)(g);
};
void __cxa_guard_release (__guard *g){
	*(char *)g = 1;
};
void __cxa_guard_abort (__guard *) {};
void __cxa_pure_virtual(void) {};
//########################################

SimpleTimer timer;

uint16_t load=0;
static uint16_t loops=0;


void updateHz(){
	load = loops;
	loops = 0;
//	BarUpdate(millis()/1000);
}

void main() __attribute__ ((noreturn));
void main(){
	{
		uint8_t mcusr = MCUSR;
		MCUSR = 0;
		if(mcusr &  _BV(WDRF)){//Watchdog System Reset Flag

		}else if(mcusr &  _BV(BORF)){//Brown-out Reset Flag

		}else if(mcusr &  _BV(EXTRF)){//External Reset Flag

		}else if(mcusr &  _BV(PORF)){//Power-on Reset Flag

		}
	}

	// Setup
	TimersInit();
	DDRB = _BV(1) | _BV(2);
	DDRC = _BV(0) | _BV(1) | _BV(2);
	DDRD = _BV(0) | _BV(1) | _BV(2);

	EEPROMload();

	timer = SimpleTimer();

	timer.setInterval(1000, updateHz);
	timer.setInterval(1, matrixDraw);
	timer.setTimeout(0, matrixGreen);
	timer.setTimeout(400, matrixRed);
	timer.setTimeout(800, matrixYellow);
	timer.setTimeout(1200, matrixClear);

	Wire.begin(1);
	Wire.onReceive(comm);
	Wire.onRequest(commRequest);

	sei();

	BarUpdate(0);

	// Loop
	uint32_t time = 0;
	bool idle = false;
	while(true){
		uint32_t t = millis();
		if(t != time){
			if(idle){
				loops++;
			}
			idle = true;
			time = t;
		}

		if(timer.run()){
			idle = false;
		}
		if(timer.getNextInvoke() >= 1){
//			if(!pumpActive() && !usbPresentRecently() && millis() - buttonPressed > 10000){
//				if(LCDisOn()){
//					LCDbacklight(0);
//					LCDcontrol(false, true, false);
//				}
//				SleepExtendedStandby();
//				if(!LCDisOn()){
//					LCDbacklight(29);
//					LCDcontrol(true, false, false);
//					buttonPressed = millis();//Fix for missed button press
//				}
//			}else{
				SleepIdle();
//			}
		}
//		matrixDraw();
		busyCheck();
	}
}


