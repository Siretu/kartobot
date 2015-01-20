/*
 * QuadratureWheels.c
 *
 *  Created on: 21 Nov 2011
 *      Author: exuvo
 */

#include "LPC17xx.h"
#include "lpc_types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "Wheels.h"
#include <math.h>

#define RISING 0
#define FALLING 1

//Pins used: 0.19 0.20 0.21 0.22 2.6 2.8 2.10 2.11 2.13
/* Front Left:	A 2.11, B 0.22
 * Front Right:	A 2.13, B 0.21
 * Rear Left:	A 2.8 , B 0.20
 * Rear Right:	A 0.19 , B 2.6
 * Button: 2.10
 */

//Quadrature Wheels Matrix  New Value
static int8_t QEM [16] = {//0  1  2  3
/*                   0  */  0,-1, 1, 2,
/*      Old Value    1  */  1, 0, 2,-1,
/*    2=error        2  */ -1, 2, 0, 1,
/*                   3  */  2, 1,-1, 0
};
static uint8_t old[4], new[4];
static int32_t pulses[4];
uint32_t wheelsFails;

void Wheels_Init(){
	uint32_t mask = (1 << 19)|(1 << 20)|(1 << 21)|(1 << 22);
	LPC_GPIOINT->IO0IntEnR = mask;
	LPC_GPIOINT->IO0IntEnF = mask;

	mask = (1 << 6)|(1 << 8)|(1 << 10)|(1 << 11)|(1 << 13);
	LPC_GPIOINT->IO2IntEnR = mask;
	LPC_GPIOINT->IO2IntEnF = mask;

	NVIC_SetPriority(EINT3_IRQn, 18);
	NVIC_EnableIRQ(EINT3_IRQn);
}

static void Wheels_Update(uint8_t motor){
	uint8_t A,B;
	if(motor == MOTOR_RR){
		A = (LPC_GPIO2->FIOPIN >> 11) & 1;
		B = (LPC_GPIO0->FIOPIN >> 22) & 1;
	}else if(motor == MOTOR_RL){
		A = (LPC_GPIO2->FIOPIN >> 13) & 1;
		B = (LPC_GPIO0->FIOPIN >> 21) & 1;
	}else if(motor == MOTOR_FR){
		A = (LPC_GPIO0->FIOPIN >> 20) & 1;
		B = (LPC_GPIO2->FIOPIN >>  8) & 1;
	}else if(motor == MOTOR_FL){
		A = (LPC_GPIO0->FIOPIN >> 19) & 1;
		B = (LPC_GPIO2->FIOPIN >>  6) & 1;
	}

	new[motor] = A * 2 + B;           // Convert binary input to decimal value
	int8_t direction = QEM[old[motor] * 4 + new[motor]];

	if(direction == 2){//Invalid value, probably missed a step
		wheelsFails++;
	}else{
		pulses[motor] += direction;
	}

	old[motor] = new[motor];
}

/* Front Left:	A 2.11, B 0.22
 * Front Right:	A 2.13, B 0.21
 * Rear Left:	A 2.8 , B 0.20
 * Rear Right:	A 2.6 , B 0.19
 * Button: 2.10 */
void EINT3_IRQHandler(){
	if(LPC_GPIOINT->IntStatus & (1 << 0)){
		if(GPIO_GetIntStatus(0, 22, RISING) || GPIO_GetIntStatus(0, 22, FALLING)){
			Wheels_Update(MOTOR_RR);
			LPC_GPIOINT->IO0IntClr = (1 << 22);
		}
		if(GPIO_GetIntStatus(0, 21, RISING) || GPIO_GetIntStatus(0, 21, FALLING)){
			Wheels_Update(MOTOR_RL);
			LPC_GPIOINT->IO0IntClr = (1 << 21);
		}
		if(GPIO_GetIntStatus(0, 20, RISING) || GPIO_GetIntStatus(0, 20, FALLING)){
			Wheels_Update(MOTOR_FR);
			LPC_GPIOINT->IO0IntClr = (1 << 20);
		}
		if(GPIO_GetIntStatus(0, 19, RISING) || GPIO_GetIntStatus(0, 19, FALLING)){
			Wheels_Update(MOTOR_FL);
			LPC_GPIOINT->IO0IntClr = (1 << 19);
		}
	}

	if(LPC_GPIOINT->IntStatus & (1 << 2)){
		if(GPIO_GetIntStatus(2, 11, RISING) || GPIO_GetIntStatus(2, 11, FALLING)){
			Wheels_Update(MOTOR_RR);
			LPC_GPIOINT->IO2IntClr = (1 << 11);
		}
		if(GPIO_GetIntStatus(2, 13, RISING) || GPIO_GetIntStatus(2, 13, FALLING)){
			Wheels_Update(MOTOR_RL);
			LPC_GPIOINT->IO2IntClr = (1 << 13);
		}
		if(GPIO_GetIntStatus(2, 8, RISING) || GPIO_GetIntStatus(2, 8, FALLING)){
			Wheels_Update(MOTOR_FR);
			LPC_GPIOINT->IO2IntClr = (1 << 8);
		}
		if(GPIO_GetIntStatus(2, 6, RISING) || GPIO_GetIntStatus(2, 6, FALLING)){
			Wheels_Update(MOTOR_FL);
			LPC_GPIOINT->IO2IntClr = (1 << 6);
		}
		if(GPIO_GetIntStatus(2, 10, RISING) || GPIO_GetIntStatus(2, 10, FALLING)){
			void Button(uint32_t state);
			Button(LPC_GPIO2->FIOPIN & (1 << 10));
			LPC_GPIOINT->IO2IntClr = (1 << 10);
		}
	}
}

void Wheels_Reset(){
	taskDISABLE_INTERRUPTS();

	for(uint8_t i=0; i < 4; i++){
		pulses[i] = 0;
	}

	taskENABLE_INTERRUPTS();
}

int32_t Wheels_GetPulses(uint8_t motor){
	return pulses[motor];
}

int32_t Wheels_GetRevolutions(uint8_t motor){
	int32_t revolutions = Wheels_GetPulses(motor)/333;
	return revolutions;
}

//Centimeters
int32_t Wheels_GetDistance(uint8_t motor){
	int32_t distance = ((double)Wheels_GetPulses(motor) / (1000/3)) * WHEEL_DIAMATER * M_PI;
	return distance;
}

double Wheels_GetDistanceD(uint8_t motor){
	return ((double)Wheels_GetPulses(motor) / (1000.0/3.0)) * WHEEL_DIAMATER * M_PI;
}

