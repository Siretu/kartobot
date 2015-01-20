/*
 * Gyro.c
 *
 *  Created on: 28 Dec 2011
 *      Author: exuvo
 */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "Gyro.h"
#include "I2C.h"
#include "term_io.h"
#include "utils.h"

#define ITG3200_ADDRESS 0x68
#define GYRO_SCALE 14.375

static int16_t raw[3];
static int16_t offset[4];
static double scaled[3];
static float temp;

static xSemaphoreHandle GYRO_lock;
uint32_t gyroFails;

//TODO auto zero when standing still

void Gyro_Update(){
	uint8_t address[1] = {27};
	uint8_t data[8];// TEMPH, TEMPL, XH, XL, YH, YL, ZH, ZL

	I2C_M_SETUP_Type s;
	s.sl_addr7bit = ITG3200_ADDRESS;
	s.tx_data = address;
	s.tx_length = 1;
	s.rx_data = data;
	s.rx_length = 8;
	s.retransmissions_max = 1;
	I2C0_sendS(&s);

	if((s.status & I2C_SETUP_STATUS_DONE) && s.rx_count >= s.rx_length){
		int16_t t,x,y,z;
		t =   data[0] << 8 | data[1];
		x =   data[2] << 8 | data[3];
		y = -(data[4] << 8 | data[5]);
		z =   data[6] << 8 | data[7];

		if(xSemaphoreTake(GYRO_lock, portMAX_DELAY) == pdTRUE){
			raw[0] = x;//-
			raw[1] = y;//-
			raw[2] = -z;
			scaled[0] = GYRO_SCALED_RAD(raw[0] - offset[0]);
			scaled[1] = GYRO_SCALED_RAD(raw[1] - offset[1]);
			scaled[2] = GYRO_SCALED_RAD(raw[2] - offset[2]);
			temp = t - offset[3];

			xSemaphoreGive(GYRO_lock);
		}
	}else{
		gyroFails++;
	}
}

void Gyro_Init(){
	GYRO_lock = xSemaphoreCreateMutex();

	uint8_t address[1] = {0};
	uint8_t id[1];

	I2C_M_SETUP_Type s;
	s.sl_addr7bit = ITG3200_ADDRESS;
	s.tx_data = address;
	s.tx_length = 1;
	s.rx_data = id;
	s.rx_length = 1;
	s.retransmissions_max = 3;
	I2C0_sendS(&s);

	if( !(s.status & I2C_SETUP_STATUS_DONE) || ((id[0]-1) != ITG3200_ADDRESS)){
		xputs("Gyro is not responding!\n");
		return;
	}

//	uint8_t reset[2] = {62,
//			0x80};//RESET
//
//	s.sl_addr7bit = ITG3200_ADDRESS;
//	s.tx_data = reset;
//	s.tx_length = 2;
//	s.rx_data = NULL;
//	s.rx_length = 0;
//	s.retransmissions_max = 3;
//	I2C0_sendS(&s);

	uint8_t pwr[2] = {62,
			0x03};//PWR_MGM = PLL with Z Gyro reference

	s.sl_addr7bit = ITG3200_ADDRESS;
	s.tx_data = pwr;
	s.tx_length = 2;
	s.rx_data = NULL;
	s.rx_length = 0;
	s.retransmissions_max = 3;
	I2C0_sendS(&s);

	uint8_t data[4] = {21,
			20,				//SMPLRT_DIV = 50Hz
			(3 << 3) | 3,	//DLPF_FS	 = internal sample rate 1kHz, 42 Hz low pass filter, +/- 2000 °/s range
			0};			//INT_CFG	 = No interrupts

	s.sl_addr7bit = ITG3200_ADDRESS;
	s.tx_data = data;
	s.tx_length = 4;
	s.rx_data = NULL;
	s.rx_length = 0;
	s.retransmissions_max = 3;
	I2C0_sendS(&s);

	//TODO do a self test
	//TODO compensate for temperature

	offset[0] = GYRO_AVERAGE_OFFSET_X;
	offset[1] = GYRO_AVERAGE_OFFSET_Y;
	offset[2] = GYRO_AVERAGE_OFFSET_Z;

	Gyro_Update();
}

float Gyro_GetTemp(){
	return temp;
}

double Gyro_Get(uint8_t axis){
	return scaled[axis];
}

int16_t Gyro_GetRaw(uint8_t axis){
	return raw[axis];
}

double Gyro_GetX(){
	return scaled[X];
}

double Gyro_GetY(){
	return scaled[Y];
}

double Gyro_GetZ(){
	return scaled[Z];
}

