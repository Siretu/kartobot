/*
 * EEPROM.c
 *
 *  Created on: 2 Sep 2011
 *      Author: exuvo
 */

#include "I2C.h"
#include <string.h>
#include "PID.h"
#include "dbg.h"

#define EEPROM_ADDRESS 0x50

Status EEPROM_write(uint16_t address, uint8_t* data, uint32_t length){
	uint8_t data2[2+length];
	data2[0] = address >> 8; data2[1] = address;
	memcpy(data2 + 2, data, length);

	return I2C0_send(EEPROM_ADDRESS, data2, length+2U);
}

Status EEPROM_read(uint16_t address, uint8_t* data, uint32_t length){
	uint8_t addres[2] = {address >> 8, address};

	I2C_M_SETUP_Type s;
	s.sl_addr7bit = EEPROM_ADDRESS;
	s.tx_data = addres;
	s.tx_length = 2;
	s.rx_data = data;
	s.rx_length = length;
	s.retransmissions_max = 3;

	return I2C0_sendS(&s);
}

Status EEPROM_readPID(uint16_t address, PIDdata *d){
	union{
		float f[3];
		uint8_t b[12];
	}u;

	Status s = EEPROM_read(address, u.b, 12);
	check(s, "EEPROM read PID error.");
	d->P = u.f[0];
	d->I = u.f[1];
	d->D = u.f[2];
	return SUCCESS;

error:
	return ERROR;
}

Status EEPROM_writePID(uint16_t address, PIDdata *d){
	union{
		float f[3];
		uint8_t b[12];
	}u;

	u.f[0] = d->P;
	u.f[1] = d->I;
	u.f[2] = d->D;

	Status s = EEPROM_write(address, u.b, 12);
	check(s, "EEPROM write PID error.");
	return SUCCESS;

error:
	return ERROR;
}
