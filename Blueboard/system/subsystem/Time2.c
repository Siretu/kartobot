/*
 * RTC.c
 *
 *  Created on: 1 Sep 2011
 *      Author: exuvo
 */

#include "LPC17xx.h"
#include "lpc17xx_rtc.h"
#include "lpc17xx_timer.h"
#include "FreeRTOS.h"
#include "Time2.h"

/* # of bits used in the Calibration value */
#define CALIBRATION_VALUE_SIZE	17
/* If the actual frequency is within +/- of this value,
	calibration should be disabled because of overflow */
#define MAX_DELTA_FREQUENCY_VALUE	(RTC_IDEAL_FREQ / ((1 << CALIBRATION_VALUE_SIZE) - 1))

//RTC_WriteGPREG(0-4) may be used for storing stuff between power cycles, if power is kept to RTC. It is not powered..
//TODO disconnect VBAT pin from VCC and add a clock battery.

void Time_Calibration(uint32_t actual_rtc_frequency);

void Time_Init(){
	TIM_TIMERCFG_Type tc;
	tc.PrescaleOption = TIM_PRESCALE_USVAL;
	tc.PrescaleValue = 1;//uS
	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &tc);

	TIM_ResetCounter(LPC_TIM0);
	TIM_Cmd(LPC_TIM0, ENABLE);


	if(!(LPC_RTC->CCR & RTC_CCR_CLKEN)){//Only reset if the timer is not running
		RTC_Init(LPC_RTC);
		LPC_RTC->RTC_AUX = RTC_AUX_RTC_OSCF;
		Time_Calibration(RTC_ACTUAL_FREQ);

		RTC_TIME_Type initial_time = {
			0, 0, 0, // SS:MM:HH
			26, 0, 0,  // DOM, DOW, DOY
			8, 2012    // MONTH, YEAR
		};

		RTC_SetFullTime (LPC_RTC, &initial_time);
		RTC_Cmd(LPC_RTC, ENABLE);
	}
}

void Time_Calibration(uint32_t actual_rtc_frequency){
	uint32_t calibration_value;
	int32_t rtc_frequency_delta;

	rtc_frequency_delta = (RTC_IDEAL_FREQ - actual_rtc_frequency);

	RTC_CalibCounterCmd(LPC_RTC, DISABLE);

	/* Check for valid RTC frequency */
	/* Disable calibration if the correction value is +/- .250Hz offset
	 It would otherwise cause an overflow in RTCCAL */
	if ((rtc_frequency_delta <= MAX_DELTA_FREQUENCY_VALUE)
	    && (rtc_frequency_delta >= -MAX_DELTA_FREQUENCY_VALUE)) {

		RTC_CalibCounterCmd(LPC_RTC, DISABLE);

		/* Determine the calibration direction CALDIR and CALVAL*/
	} else if (rtc_frequency_delta > MAX_DELTA_FREQUENCY_VALUE) {

		/* Forward direction */
		calibration_value = RTC_IDEAL_FREQ / (RTC_IDEAL_FREQ - actual_rtc_frequency);
		RTC_CalibConfig(LPC_RTC, calibration_value, RTC_CALIB_DIR_FORWARD);
		RTC_CalibCounterCmd(LPC_RTC, ENABLE);

	} else {

		/* Backward direction */
		calibration_value = RTC_IDEAL_FREQ / (actual_rtc_frequency - RTC_IDEAL_FREQ);
		RTC_CalibConfig(LPC_RTC, calibration_value, RTC_CALIB_DIR_BACKWARD);
		RTC_CalibCounterCmd(LPC_RTC, ENABLE);

	}

	return;
}

time_t Time_UNIX(){
	struct tm tmp;
	RTC_TIME_Type t;

	RTC_GetFullTime(LPC_RTC, &t);
	tmp.tm_year = t.YEAR;
	tmp.tm_mon  = t.MONTH;
	tmp.tm_mday = t.DOM;
	tmp.tm_hour = t.HOUR;
	tmp.tm_min  = t.MIN;
	tmp.tm_sec  = t.SEC;

	return mktime(&tmp);
}

uint32_t Time_Micros(){
	return LPC_TIM0->TC;
}

uint32_t Time_Millis(){
	return LPC_TIM0->TC / 1000;
}

void Time_Delay(uint32_t us){
	uint32_t start = Time_Micros();
	while(Time_Micros() - start < us);
}

void Time_SetTime(RTC_TIME_Type *pFullTime){
	RTC_Cmd(LPC_RTC, DISABLE);
	RTC_SetFullTime(LPC_RTC, pFullTime);
	RTC_Cmd(LPC_RTC, ENABLE);
}

uint8_t Time_GetTime(RTC_TIME_Type *pFullTime){
	uint32_t ct0, ct1;

	// adapted from ChaN's FatFs LPC23xx example:
	do {
		ct0 = LPC_RTC->CTIME0;
		ct1 = LPC_RTC->CTIME1;
	} while (ct0 != LPC_RTC->CTIME0 || ct1 != LPC_RTC->CTIME1);

	pFullTime->SEC   = ct0 & 63;;
	pFullTime->MIN   = (ct0 >> 8) & 63;;
	pFullTime->HOUR  = (ct0 >> 16) & 31;
	pFullTime->DOM   = ct1 & 31;
	pFullTime->MONTH = (ct1 >> 8) & 15;;
	pFullTime->YEAR  = (ct1 >> 16) & 4095;

	return (LPC_RTC->RTC_AUX & RTC_AUX_RTC_OSCF) ? 1 : 0;
}
