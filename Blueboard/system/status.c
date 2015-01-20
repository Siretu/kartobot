/*
 * status.c
 *
 *  Created on: 17 Mar 2012
 *      Author: exuvo
 */

#include "status.h"
#include "utils.h"
#include "main.h"
#include "term_io.h"
#include "XBee.h"
#include <string.h>

struct P{
	xTaskHandle p;
	uint32_t oldCounter;
};
typedef struct P P;

static portSTACK_TYPE statusTaskStack[128];
const xTaskParameters statusTaskDefinition =
{
		Status_Run,   							/* pvTaskCode */
		(signed char *) "status",   /* pcName */
		128,             						/* usStackDepth - defined in words, not bytes. */
    NULL,            						/* pvParameters */
    9 | portPRIVILEGE_BIT,      /* uxPriority */
    statusTaskStack,      			/* puxStackBuffer - the array to use as the task stack. */
    {
        {0, 0, 0}
    }
};

void Status_Run(void *pvParameters){
	portTickType xLastWakeTime = xTaskGetTickCount();

	uint8_t size = uxTaskGetNumberOfTasks();

	P p[size];
	memset(&p, 0, sizeof(p));

	for(uint8_t id=0; id<size; id++){
		xTaskHandle h = pxTaskGetByID(id);
		p[id].p = h;
	}

	{
		uint8_t d[] = {'i', 0xFF};
		XBee_Send(d, sizeof(d));
	}
	{
		uint8_t d[MAX_FRAME_DATA];
		d[0] = 'i';
		uint8_t c=1;

		for(uint8_t i=0; i < size; i++){
			if(p[i].p != NULL){
				xTaskHandle t = p[i].p;

				d[c++] = uxTaskGetID(t);
				const signed char *name = pcTaskGetTaskName(t);
				while(*name){
					d[c++] = *name++;
				}
				d[c++] = 0;

				if(c + configMAX_TASK_NAME_LEN + 1U > sizeof(d)){
					XBee_Send(d, c);
					c = 1;
				}
			}
		}
		if(c > 1){
			XBee_Send(d, c);
		}
	}

	while(1){
		uint8_t d[MAX_FRAME_DATA];
		d[0] = 'S';
		uint8_t c=1;

		uint32_t total = LPC_TIM1->TC;

		for(uint8_t i=0; i < size; i++){
			if(p[i].p != NULL){
				xTaskHandle t = p[i].p;
				d[c++] = uxTaskGetID(t);

				uint32_t counter = uxTaskGetRunTimeCounter(t);
				uint16_t time = (counter - p[i].oldCounter) / 10;//100000 -> 10000
				p[i].oldCounter = counter;

				d[c++] = time >> 8;
				d[c++] = time;

				if(total == 0){
					d[c++] = 0;
				}else{
					d[c++] = (255UL*uxTaskGetRunTimeCounter(t)) / total;
				}

				if(c + 4U > sizeof(d)){
					XBee_Send(d, c);
					c = 1;
				}
			}
		}
		if(c > 1){
			XBee_Send(d, c);
		}

		vTaskDelayUntil(&xLastWakeTime, 1000);
	}
}

