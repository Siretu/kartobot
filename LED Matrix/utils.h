/*
 * utils.h
 *
 *  Created on: 15 Mar 2012
 *      Author: exuvo
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <inttypes.h>
#include <avr/io.h>
#include <stdlib.h>
#include <math.h>

#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105

// undefine stdlib's abs if encountered
#ifdef abs
#undef abs
#endif

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define abs(x) ((x)>0?(x):-(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// Analog Reference Value
// This value provided from Configurator
// Use a DMM to measure the voltage between AREF and GND
// Enter the measured voltage below to define your value for aref
#define aref 5.0f	//Measured value

#ifdef __cplusplus
extern "C" {
#endif

uint16_t reverse(uint16_t x);
void shift(volatile uint8_t *port, uint8_t SCK_Pin, uint8_t RCK_Pin, uint8_t SER_Pin, uint32_t data, uint8_t bits);

void error(uint8_t errorCode);
void warn(uint8_t errorCode);
uint16_t freeRAM();

#ifdef __cplusplus
}
#endif
#endif /* UTILS_H_ */
