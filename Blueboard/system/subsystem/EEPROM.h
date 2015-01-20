/*
 * EEPROM.c
 *
 *  Created on: 2 Sep 2011
 *      Author: exuvo
 */

#ifndef EEPROM_H_
#define EEPROM_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "lpc_types.h"
#include "PID.h"

#define EEPROM_SIZE_INT8 	1
#define EEPROM_SIZE_INT16 2
#define EEPROM_SIZE_INT32 4
#define EEPROM_SIZE_INT64 8
#define EEPROM_SIZE_FLOAT 4

#define EEPROM_ID 0
#define EEPROM_POS_PIDL 1
#define EEPROM_POS_PIDR 13

#define EEPROM_END 32768

Status EEPROM_write(uint16_t address, uint8_t* data, uint32_t length);
Status EEPROM_read(uint16_t address, uint8_t* data, uint32_t length);
Status EEPROM_readPID(uint16_t address, PIDdata *d);
Status EEPROM_writePID(uint16_t address, PIDdata *d);

#ifdef __cplusplus
}
#endif
#endif /* EEPROM_H_ */
