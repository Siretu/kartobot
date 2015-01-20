/*
 * utils.h
 *
 *  Created on: 10 okt 2011
 *      Author: Erik
 */

#ifndef UTILS_H_
#define UTILS_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "lpc_types.h"
#include <math.h>

#define X 0
#define Y 1
#define Z 2

#define ROLL 0
#define PITCH 1
#define YAW 2

#define _BV(x) (1 << (x))

#define b2(x)   (   (x) | (   (x) >> 1) )
#define b4(x)   ( b2(x) | ( b2(x) >> 2) )
#define b8(x)   ( b4(x) | ( b4(x) >> 4) )
#define b16(x)  ( b8(x) | ( b8(x) >> 8) )
#define b32(x)  (b16(x) | (b16(x) >>16) )
#define next_power_of_2(x)      (b32(x-1) + 1)

#define ARRAY_LENGTH(_array) (sizeof(_array)/sizeof(_array[0]))

#define TO_RAD(x) ((x) * (M_PI / 180.0))
#define TO_DEG(x) ((x) * (180.0 / M_PI))

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

struct mean {
	uint8_t prev;
	uint8_t size;
	float *a;
};

long map(long x, long in_min, long in_max, long out_min, long out_max);
double mapD(double x, double in_min, double in_max, double out_min, double out_max);
uint32_t upper_power_of_two(uint32_t v);
float smooth(float currentData, float previousData, float smoothFactor);
double smoothD(double currentData, double previousData, double smoothFactor);
float mean(float newData, struct mean *s);

#ifdef __cplusplus
}
#endif
#endif /* UTILS_H_ */
