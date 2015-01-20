/*
 * Time.h
 *
 *  Created on: 6 Oct 2011
 *      Author: exuvo
 */

#ifndef TIME_H_
#define TIME_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "lpc_types.h"
#include "lpc17xx_rtc.h"
#include <time.h>

/* Definitions needed to calculate the calibration value and direction */
#define RTC_IDEAL_FREQ        32768000     //in mHz (milli Hz) (=32.768kHz)
#define RTC_ACTUAL_FREQ       32768000


void Time_Init();
time_t Time_UNIX();
uint32_t Time_Millis();
uint32_t Time_Micros();
void Time_Delay(uint32_t us);
void Time_SetTime(RTC_TIME_Type *pFullTime);
uint8_t Time_GetTime(RTC_TIME_Type *pFullTime);

#ifdef __cplusplus
}
#endif
#endif /* TIME_H_ */
