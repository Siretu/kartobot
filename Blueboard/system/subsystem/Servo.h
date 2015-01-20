/*
 * servopwm.h
 *
 *  Created on: 24 mar 2012
 *      Author: per
 */

#ifndef SERVOPWM_H_
#define SERVOPWM_H_

#include "lpc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void Servo_Init();
void Servo_SetDuty(uint8_t channel, uint16_t value);

#ifdef __cplusplus
}
#endif
#endif /* SERVOPWM_H_ */
