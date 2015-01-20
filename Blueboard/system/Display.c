/*
 * Display.c
 *
 *  Created on: 28 Mar 2012
 *      Author: exuvo
 */

#include "lpc_types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "utils.h"
#include "I2C.h"
#include "Display.h"
#include <string.h>
#include "navigation.h"
#include "AHRS.h"

#define DISPLAY_ADDRESS 0x1

const uint16_t barMap[11] = {0,1,3,7,15,31,63,127,255,511,1023};
static uint16_t matrix[8];
static uint16_t bar;
static int16_t rotation;

//static const uint16_t a1[] = {0b0000001010000000,
//							  0b0000100000100000,
//							  0b0010110000111000,
//							  0b1000000101000010,
//							  0b1000000101000010,
//							  0b0010110000111000,
//							  0b0000100000100000,
//							  0b0000001010000000};
//
//static const uint16_t a2[] = {0b0000001010000000,
//							  0b0000100000100000,
//							  0b0010110101111000,
//							  0b1000010000010010,
//							  0b1000010000010010,
//							  0b0010110101111000,
//							  0b0000100000100000,
//							  0b0000001010000000};

static portSTACK_TYPE displayTaskStack[128];
const xTaskParameters displayTaskDefinition =
{
	Display_Run,   				/* pvTaskCode */
	(signed char *) "display",  /* pcName */
    128,             			/* usStackDepth - defined in words, not bytes. */
    NULL,            			/* pvParameters */
    3 | portPRIVILEGE_BIT,      /* uxPriority */
    displayTaskStack,      		/* puxStackBuffer - the array to use as the task stack. */
     {
        {0, 0, 0}
    }
};

//TODO look at TI-84 FreeTrack assembly code for drawing routines

void Display_Clear(){
	memset(matrix, 0, sizeof(matrix));
}

void Display_SetPixel(uint8_t x, uint8_t y, uint8_t color){
	int8_t nx = x, ny = y;
	int16_t r = rotation % 360;
	if(r < 0) r += 360;

	if(r == 270){
		nx = x;
		ny = y;
	}if(r == 0){
		nx = 7-y;
		ny = x;
	}else if(r == 90){
		nx = 7-x;
		ny = 7-y;
	}else if(r == 180){
		nx = y;
		ny = 7-x;
	}

	uint16_t row = matrix[ny];
	row &= ~(_BV(nx*2) | _BV(nx*2 +1));

	if(color & GREEN){
		row |= _BV(nx*2+1);
	}
	if(color & RED){
		row |= _BV(nx*2);
	}
	matrix[ny] = row;
}

static uint8_t validSquare(int8_t x, int8_t y){
	return (x >= 0 && x < GRIDSIZE && y >= 0 && y < GRIDSIZE);
}

void Display_FaultError(uint32_t codePos, uint32_t memPos, uint16_t faultType){
	I2C_Stop(LPC_I2C1);
	Display_Clear();
	rotation = 0;

	uint8_t i=0;
	while(i<32){
		if(codePos & _BV(31 - i)){
			Display_SetPixel(i % 8, i / 8, RED);
		}else{
			Display_SetPixel(i % 8, i / 8, BLACK);
		}
		i++;
	}

	i=0;
	while(i<32){
		if(memPos & _BV(31 - i)){
			Display_SetPixel(i % 8, 4 + i / 8, GREEN);
		}else{
			Display_SetPixel(i % 8, 4 + i / 8, BLACK);
		}
		i++;
	}

	bar = faultType;

	uint8_t data[20];
	data[0] = 'b'; data[1] = bar >> 8; data[2] = bar;
	data[3] = 'm';
	uint8_t c=4;
	for(i=0; i<8; i++){
		data[c++] = matrix[i] >> 8;
		data[c++] = matrix[i];
	}

	I2C_M_SETUP_Type s;
	s.sl_addr7bit = DISPLAY_ADDRESS;
	s.tx_data = data;
	s.tx_length = sizeof(data);
	s.rx_data = 0;
	s.rx_length = 0;
	s.retransmissions_max = 1;

	I2C_MasterTransferData(LPC_I2C1, &s, I2C_TRANSFER_POLLING);
}

void Display_Run(void *pvParameters){
	portTickType xLastWakeTime = xTaskGetTickCount();
//	portTickType xLastUpdate = xLastWakeTime;

	uint8_t frame = 0;
	while(1){
		frame++;

//		if(xLastWakeTime - xLastUpdate > 500){
//			static uint8_t a = 0;
//
//			if(a == 0){
//				memcpy(matrix, a1, sizeof(matrix));
//			}else if(a == 1){
//				memcpy(matrix, a2, sizeof(matrix));
//			}
//
//			a++;
//			a = a % 2;
//			xLastUpdate = xLastWakeTime;
//		}

		{
			Display_Clear();
			Point pos;
			Position_GetXY(&pos);
			pos.x /= 10; pos.y /= 10;

			double yaw = TO_DEG(AHRS_GetYaw());

			if(yaw > -45 && yaw <= 45){
				rotation = 0;
			}else if(yaw > 45 && yaw <= 135){
				rotation = 90;
			}else if(yaw > -135 && yaw <= -45){
				rotation = 270;
			}else if(yaw > 135 || yaw <= -135){
				rotation = 180;
			}

			for(int8_t x=pos.x - 4; x < pos.x+4; x++){
				for(int8_t y=pos.y - 4; y < pos.y+4; y++){
					if(validSquare(x, y)){
						Display_SetPixel(x - (pos.x - 4), y - (pos.y - 4), grid[x][y] >= 5 ? RED : BLACK);
					}else{
						Display_SetPixel(x - (pos.x - 4), y - (pos.y - 4), YELLOW);
					}
				}
			}

			Display_SetPixel(3, 3, GREEN);
			Display_SetPixel(3, 4, GREEN);
			Display_SetPixel(4, 3, GREEN);
			Display_SetPixel(4, 4, GREEN);
		}

		uint8_t data[20];
		data[0] = 'b'; data[1] = bar >> 8; data[2] = bar;
		data[3] = 'm';
		uint8_t c=4;
		for(uint8_t i=0; i<8; i++){
			data[c++] = matrix[i] >> 8;
			data[c++] = matrix[i];
		}

		I2C_M_SETUP_Type s;
		s.sl_addr7bit = DISPLAY_ADDRESS;
		s.tx_data = data;
		s.tx_length = sizeof(data);
		s.rx_data = 0;
		s.rx_length = 0;
		s.retransmissions_max = 1;
		I2C1_sendS(&s);

		vTaskDelayUntil(&xLastWakeTime, 200);
	}
}

void Display_SetBar(uint16_t value){
	bar = value;
}
