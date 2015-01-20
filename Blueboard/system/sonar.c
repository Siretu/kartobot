/*
 * Radar.c
 *
 *  Created on: 13 Sep 2011
 *      Author: exuvo
 */

#include "lpc_types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sonar.h"
#include "queue.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_pinsel.h"
#include <math.h>
#include "utils.h"
#include "Magneto.h"
#include "Servo.h"
#include "AHRS.h"

#define FRONT 0
#define BACK 1

#define CH 0
#define SPEED 5

xQueueHandle newPoints;
uint32_t sonarUnresonable;
uint32_t sonarQueueFull;
uint32_t sonarLeft;
uint32_t sonarRight;

typedef struct{
	uint16_t angle;
	int16_t botAngle;
	volatile uint32_t pings;
}sonarVars;

static sonarVars sv;

static portSTACK_TYPE sonarTaskStack[128];
const xTaskParameters sonarTaskDefinition =
{
	Sonar_Run,   				/* pvTaskCode */
	(signed char *) "sonar",  	/* pcName */
    128,             			/* usStackDepth - defined in words, not bytes. */
    NULL,            			/* pvParameters */
    5,     						/* uxPriority */
    sonarTaskStack,      		/* puxStackBuffer - the array to use as the task stack. */
    {
        {0, 0, 0}
    }
};

void Sonar_Run(void *pvParameters){
	uint16_t ang = 500;
	int8_t direction = 1;

//	LPC_GPIO1->FIOSET = (1 << 24) | (1 << 25);//Start ranging
	while(1){
		if(LPC_GPIO1->FIOPIN & ((1 << 24) | (1 << 25))){
			sv.angle = ang;
			sv.botAngle = AHRS_GetYaw() * 8192;

			if(direction == 1){
				if(ang < MAXPOS){
					ang += SPEED;
				}else{
					direction = -1;
				}
			}else if(direction == -1){
				if(ang > MINPOS){
					ang -= SPEED;
				}else{
					direction = 1;
				}
			}

			if(ang > MAXPOS)ang = MAXPOS;
			if(ang < MINPOS)ang = MINPOS;

			Servo_SetDuty(CH, ang);
			vTaskDelay(10);//100Hz
		}else{
			Servo_SetDuty(CH, (MAXPOS-MINPOS)/2);
			vTaskSuspend(NULL);
		}
	}
}

void Sonar_Init(){
	newPoints = xQueueCreate(200, sizeof(Ping));

	TIM_CAPTURECFG_Type cc;
	cc.RisingEdge = ENABLE;
	cc.FallingEdge = ENABLE;
	cc.IntOnCaption = ENABLE;
	cc.CaptureChannel = 0;
	TIM_ConfigCapture(LPC_TIM0, &cc);
	cc.CaptureChannel = 1;
	TIM_ConfigCapture(LPC_TIM0, &cc);

	NVIC_SetPriority(TIMER0_IRQn, 15);
	NVIC_EnableIRQ(TIMER0_IRQn);

//	//Config P1.26, 27 as CAP0.0, CAP0.1
	PINSEL_CFG_Type pin;
	pin.OpenDrain = PINSEL_PINMODE_NORMAL;
	pin.Pinmode = PINSEL_PINMODE_PULLDOWN;
	pin.Funcnum = 3;
	pin.Portnum = 1;
	pin.Pinnum = 26;
	PINSEL_ConfigPin(&pin);
	pin.Pinnum = 27;
	PINSEL_ConfigPin(&pin);

	//Config P1.24, 25 as GPIO for controlling ranging. 5V pullups in the sensors
	pin.OpenDrain = PINSEL_PINMODE_OPENDRAIN;
	pin.Pinmode = PINSEL_PINMODE_PULLDOWN;
	pin.Funcnum = 0;
	pin.Portnum = 1;
	pin.Pinnum = 24;
	PINSEL_ConfigPin(&pin);
	pin.Pinnum = 25;
	PINSEL_ConfigPin(&pin);

	LPC_GPIO1->FIOCLR |= (1 << 24) | (1 << 25);
	LPC_GPIO1->FIODIR |= (1 << 24) | (1 << 25);//Outputs
}

static void newPoint(uint32_t pulseWidth, int8_t sensor, portBASE_TYPE *xHigherPriorityTaskWoken){
	Ping p;

	uint32_t length = pulseWidth / 58; // In centimeters
//	if(length < 10 || length > 765){//Unreasonable value
//		sonarUnresonable++;
//		return;
//	}
	if(length < 8 || length > 150){//Unwanted
		return;
	}

	double ang = mapD(sv.angle, MINPOS, MAXPOS, -M_PI_2, M_PI_2) + sensor * M_PI - sv.botAngle/8192.0;

	p.x = sin(ang) * length;
	p.y = cos(ang) * length;

	sv.pings++;
	if(xQueueSendToBackFromISR(newPoints, &p, xHigherPriorityTaskWoken) == errQUEUE_FULL){
		sonarQueueFull++;
	}
}

void TIMER0_IRQHandler(void){
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	static uint32_t start0, start1;

	if (TIM_GetIntStatus(LPC_TIM0, TIM_CR0_INT)){
		TIM_ClearIntPending(LPC_TIM0, TIM_CR0_INT);
		uint32_t value = LPC_TIM0->CR0;

		if(LPC_GPIO1->FIOPIN & (1 << 26)){//Begin pulse
			start0 = value;
		}else{//End pulse
			newPoint(value - start0, FRONT, &xHigherPriorityTaskWoken);
		}
//		sonarLeft++;
	}

	if (TIM_GetIntStatus(LPC_TIM0, TIM_CR1_INT)){
		TIM_ClearIntPending(LPC_TIM0, TIM_CR1_INT);
		uint32_t value = LPC_TIM0->CR1;

		if(LPC_GPIO1->FIOPIN & (1 << 27)){//Begin pulse
			start1 = value;
		}else{//End pulse
			newPoint(value - start1, BACK, &xHigherPriorityTaskWoken);
		}
//		sonarRight++;
	}

	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

