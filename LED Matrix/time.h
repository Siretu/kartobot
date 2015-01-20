/*
 * time.h
 *
 *  Created on: 21 Jul 2011
 *      Author: exuvo
 */

#ifndef TIME_H_
#define TIME_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L ) //16
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )

uint32_t millis();
uint32_t micros();
void delay(uint32_t);
void TimersInit();

#ifdef __cplusplus
}
#endif
#endif /* TIME_H_ */
