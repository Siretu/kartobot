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

#include "main.h"
#include "EEPROM.h"
#include <avr/eeprom.h>
#include <avr/interrupt.h>

float EEMEM PIDSP[9];

// Utilities for writing and reading from the EEPROM
static float readFloat(float* address){
	cli();
	float f = eeprom_read_float(address);
	sei();
	return f;
}

static void writeFloat(float* address, const float value) {
	cli();
    eeprom_update_float(address, value);
    sei();
}

static int16_t readInt(int16_t* address){
	cli();
	int16_t f = eeprom_read_word((uint16_t*)address);
	sei();
	return f;
}

static void writeInt(int16_t* address, const int16_t value) {
	cli();
	eeprom_update_word((uint16_t*)address, value);
	sei();
}

// contains all default values when re-writing EEPROM
void EEPROMreset() {
	//minAcro = 300;
}

void EEPROMload() {
	//windupGuard = readFloat(&WINDUPGUARD);
}

void EEPROMsave(void){
	//writeFloat(&TIMECONSTANT, timeConstant);
}
