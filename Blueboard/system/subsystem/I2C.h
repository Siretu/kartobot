/*
 * I2C.h
 *
 *  Created on: 10 Sep 2011
 *      Author: exuvo
 */

#ifndef I2C_H_
#define I2C_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "lpc17xx_i2c.h"

void I2CS_Init();
Status I2C0_sendS(I2C_M_SETUP_Type *TransferCfg);
Status I2C0_send(uint8_t address, uint8_t *data, uint32_t length);
Status I2C1_sendS(I2C_M_SETUP_Type *TransferCfg);
Status I2C1_send(uint8_t address, uint8_t *data, uint32_t length);
void I2C1_receive();
void I2C0_Test();

#ifdef __cplusplus
}
#endif
#endif /* I2C_H_ */
