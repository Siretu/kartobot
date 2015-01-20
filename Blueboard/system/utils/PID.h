/*
 * PID.h
 *
 *  Created on: 4 nov 2010
 *      Author: exuvo
 */

#ifndef PID_H_
#define PID_H_

#include "lpc_types.h"

struct PIDdata {
	float P, I, D;
	float lastPosition;
	float integratedError;
	float previousPIDTime;
	float windupGuard;
	const char  *name;//max 16
	const uint16_t eeprom;
};
typedef struct PIDdata PIDdata;

float PID_Update(float targetPosition, float currentPosition, PIDdata *PIDparameters) ;
void PID_ZeroIntegralError(PIDdata *PIDparameters) ;
uint8_t PID_Length();
void PID_Load(PIDdata *p);
void PID_Save(PIDdata *p);
PIDdata* PID_Get(uint8_t id);
void PID_Set(PIDdata *p, float P, float I, float D);

#endif /* PID_H_ */
