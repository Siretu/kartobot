/*
 * DataStorage.h
 *
 *  Created on: 4 nov 2010
 *      Author: exuvo
 */

#ifndef DATASTORAGE_H_
#define DATASTORAGE_H_

#include <inttypes.h>
#include <avr/eeprom.h>

extern float EEMEM PIDSP[9];

void EEPROMreset(void);
void EEPROMload();
void EEPROMsave(void);

#endif /* DATASTORAGE_H_ */
