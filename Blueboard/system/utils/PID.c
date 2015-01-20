/*
  AeroQuad v2.0.1 - September 2010
  www.AeroQuad.com
  Copyright (c) 2010 Ted Carancho.  All rights reserved.
  An Open Source Arduino based multicopter.
 
  This program is free software: you can redistribute it and/or modify 
  it under the terms of the GNU General Public License as published by 
  the Free Software Foundation, either version 3 of the License, or 
  (at your option) any later version. 

  This program is distributed in the hope that it will be useful, 
  but WITHOUT ANY WARRANTY; without even the implied warranty of 
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
  GNU General Public License for more details. 

  You should have received a copy of the GNU General Public License 
  along with this program. If not, see <http://www.gnu.org/licenses/>. 
*/

#include "FreeRTOS.h"
#include "task.h"
#include "PID.h"
#include "utils.h"
#include "Time2.h"
#include "EEPROM.h"

#define PID_LENGTH 2

extern PIDdata PIDL, PIDR;
PIDdata *PIDs[] = { &PIDL, &PIDR };

float PID_Update(float targetPosition, float currentPosition, PIDdata *PIDparameters) {
	uint32_t currentTime = Time_Micros();
	float deltaPIDTime = (currentTime - PIDparameters->previousPIDTime) / 1000000.0f;
	PIDparameters->previousPIDTime = currentTime;

	float error = targetPosition - currentPosition;

	PIDparameters->integratedError += error * deltaPIDTime;
	PIDparameters->integratedError = constrain(PIDparameters->integratedError, -PIDparameters->windupGuard, PIDparameters->windupGuard);

	float dTerm = PIDparameters->D * (currentPosition - PIDparameters->lastPosition) / (deltaPIDTime * 100);
	PIDparameters->lastPosition = currentPosition;
	return (PIDparameters->P * error) + (PIDparameters->I * (PIDparameters->integratedError)) + dTerm;
}

void PID_ZeroIntegralError(PIDdata *PIDparameters) {
	uint32_t currentTime = Time_Micros();
	PIDparameters->integratedError = 0;
	PIDparameters->previousPIDTime = currentTime;
}

uint8_t PID_Length(){
	return PID_LENGTH;
}

void PID_Load(PIDdata *p){
	EEPROM_readPID(p->eeprom, p);
}

void PID_Save(PIDdata *p){
	EEPROM_writePID(p->eeprom, p);
}

PIDdata* PID_Get(uint8_t id){
	PIDdata *r = 0;

	if(id < PID_LENGTH){
		r = PIDs[id];
	}

	return r;
}

void PID_Set(PIDdata *p, float P, float I, float D){
	p->P = P;
	p->I = I;
	p->D = D;
}

