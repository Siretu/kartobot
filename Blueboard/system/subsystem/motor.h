/*
 * motor.h
 *
 *  Created on: Oct 13, 2011
 *      Author: per
 */

#ifndef MOTOR_H_
#define MOTOR_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "lpc_types.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_pwm.h"
#include "LPC17xx.h"
#include "lpc17xx_clkpwr.h"

#define SIDE_LEFT 		0
#define SIDE_RIGHT 		1

#define GO_FORWARD 		0
#define GO_BACK 		1
#define GO_STOP			2

void Motor_Init();
void Motor_Control(uint8_t motorSide, uint8_t dir);
void Motor_ControlSpeed(uint8_t motorSide, uint16_t vel);
void Motor_Disable();
void Motor_Enable();
uint8_t Motor_Enabled();

#ifdef __cplusplus
}
#endif
#endif /* MOTOR_H_ */
