/*
 * Accelerometer.c
 *
 *  Created on: 28 Dec 2011
 *      Author: exuvo
 */

//http://code.google.com/p/aeroquad/source/browse/branches/KennyIntegration/Libraries/AQ_Accelerometer/ADXL345.h?r=767
//http://forum.sparkfun.com/viewtopic.php?f=14&t=30514
//http://forum.sparkfun.com/viewtopic.php?f=14&t=29081

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "Accel.h"
#include "I2C.h"
#include "term_io.h"
#include "utils.h"

#define ADXL345_ADDRESS 0x53

static int16_t raw[3];
static int16_t offset[3];
static double scaled[3];

static xSemaphoreHandle ACCEL_lock;
uint32_t accelFails;

void Accel_Update(){
	uint8_t address[1] = {0x32};
	uint8_t data[6];// XH, XL, YH, YL, ZH, ZL

	I2C_M_SETUP_Type s;
	s.sl_addr7bit = ADXL345_ADDRESS;
	s.tx_data = address;
	s.tx_length = 1;
	s.rx_data = data;
	s.rx_length = 6;
	s.retransmissions_max = 1;
	I2C0_sendS(&s);

	if((s.status & I2C_SETUP_STATUS_DONE) && s.rx_count >= s.rx_length){
		int16_t x,y,z;
		x = data[1] << 8 | data[0];
		y = data[3] << 8 | data[2];
		z = data[5] << 8 | data[4];

		//13bit
		if(x < -0x7FF || x > 0x7FF || y < -0x7FF || y > 0x7FF || z < -0x7FF || z > 0x7FF){
			accelFails++;
		}else{
			if(xSemaphoreTake(ACCEL_lock, portMAX_DELAY) == pdTRUE){
				raw[0] = -x;
				raw[1] = y;
				raw[2] = z;
				scaled[0] = smoothD((raw[0] - offset[0]) * ACCEL_X_SCALE, scaled[0], 0.8);
				scaled[1] = smoothD((raw[1] - offset[1]) * ACCEL_Y_SCALE, scaled[1], 0.8);
				scaled[2] = smoothD((raw[2] - offset[2]) * ACCEL_Z_SCALE, scaled[2], 0.8);

				xSemaphoreGive(ACCEL_lock);
			}
		}
	}else{
		accelFails++;
	}
}

void Accel_Init(){
	ACCEL_lock = xSemaphoreCreateMutex();

	uint8_t address[1] = {0};
	uint8_t id[1];

	I2C_M_SETUP_Type s;
	s.sl_addr7bit = ADXL345_ADDRESS;
	s.tx_data = address;
	s.tx_length = 1;
	s.rx_data = id;
	s.rx_length = 1;
	s.retransmissions_max = 3;
	I2C0_sendS(&s);

	if( !(s.status & I2C_SETUP_STATUS_DONE) || (id[0] != 0xE5)){
		xputs("Accel is not responding!\n");
		return;
	}

	uint8_t data[3] = {0x2C,
			0b1001,	//50Hz output rate, 25Hz bandwidth
			1<<3};	//Set device to measure

	s.sl_addr7bit = ADXL345_ADDRESS;
	s.tx_data = data;
	s.tx_length = 3;
	s.rx_data = NULL;
	s.rx_length = 0;
	s.retransmissions_max = 3;
	I2C0_sendS(&s);

	data[0] = 0x31;
	data[1] = 1<<3;	//Full resolution

	s.sl_addr7bit = ADXL345_ADDRESS;
	s.tx_data = data;
	s.tx_length = 2;
	s.rx_data = NULL;
	s.rx_length = 0;
	s.retransmissions_max = 3;
	I2C0_sendS(&s);

	data[0] = 0x38;
	data[1] = 1<<7;	//FIFO in Stream mode

	s.sl_addr7bit = ADXL345_ADDRESS;
	s.tx_data = data;
	s.tx_length = 2;
	s.rx_data = NULL;
	s.rx_length = 0;
	s.retransmissions_max = 3;
	I2C0_sendS(&s);

	//TODO do a self test

	offset[0] = ACCEL_X_OFFSET;
	offset[1] = ACCEL_Y_OFFSET;
	offset[2] = ACCEL_Z_OFFSET;

	Accel_Update();
}

double Accel_Get(uint8_t axis){
	return scaled[axis];
}

int16_t Accel_GetRaw(uint8_t axis){
	return raw[axis];
}

double Accel_GetX(){
	return scaled[X];
}

double Accel_GetY(){
	return scaled[Y];
}

double Accel_GetZ(){
	return scaled[Z];
}

double Accel_OneG(){
	return GRAVITY;
}

