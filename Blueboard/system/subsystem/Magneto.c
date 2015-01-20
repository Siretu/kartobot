/*
 * Magneto.c
 *
 *  Created on: 28 Dec 2011
 *      Author: exuvo
 */
//http://mbed.org/users/BlazeX/libraries/HMC5883/m10aqh/docs/HMC5883_8cpp_source.html
#include "FreeRTOS.h"
#include "semphr.h"
#include "Magneto.h"
#include "I2C.h"
#include "term_io.h"
#include "math.h"
#include "utils.h"
#include "AHRS.h"

#define HMC5843_ADDRESS 0x1E
#define MINV -2048
#define MAXV 2047

static int16_t raw[3];
static int16_t offset[3];
static double scaled[3];
static double heading;

static xSemaphoreHandle MAG_lock;
uint32_t magnetoOverflow, magnetoFails;

void Magneto_Update(){
	uint8_t address[1] = {3};
	uint8_t data[6];

	I2C_M_SETUP_Type s;
	s.sl_addr7bit = HMC5843_ADDRESS;
	s.tx_data = address;
	s.tx_length = 1;
	s.rx_data = data;
	s.rx_length = 6;
	s.retransmissions_max = 1;
	I2C0_sendS(&s);

	if((s.status & I2C_SETUP_STATUS_DONE) && s.rx_count >= s.rx_length){
			int16_t x,y,z;
			x = data[0] << 8 | data[1];
			y = data[2] << 8 | data[3];
			z = data[4] << 8 | data[5];

			if(x == -4096 || y == -4096 || z == -4096){
				magnetoOverflow++;
			}else if(x < MINV || x > MAXV || y < MINV || y > MAXV || z < MINV || z > MAXV){
				magnetoFails++;
			}else{
				if(xSemaphoreTake(MAG_lock, portMAX_DELAY) == pdTRUE){
					raw[0] = y;
					raw[1] = x;
					raw[2] = -z;
					scaled[0] = smoothD((raw[0] - offset[0]) * MAGN_X_SCALE, scaled[0], 0.5);
					scaled[1] = smoothD((raw[1] - offset[1]) * MAGN_Y_SCALE, scaled[1], 0.5);
					scaled[2] = smoothD((raw[2] - offset[2]) * MAGN_Z_SCALE, scaled[2], 0.5);

					double cos_roll = cos(AHRS_GetRoll());
					double sin_roll = sin(AHRS_GetRoll());
					double cos_pitch = cos(AHRS_GetPitch());
					double sin_pitch = sin(AHRS_GetPitch());

					// Tilt compensated magnetic field X
					double mag_x = scaled[0]*cos_pitch + scaled[1]*sin_roll*sin_pitch + scaled[2]*cos_roll*sin_pitch;

					// Tilt compensated magnetic field Y
					double mag_y = scaled[1]*cos_roll - scaled[2]*sin_roll;

					// Magnetic Heading
					heading = atan2(-mag_y, mag_x);

					xSemaphoreGive(MAG_lock);
				}
			}
	}else{
		magnetoFails++;
	}
}

void Magneto_Init(){
	//_delay_ms(56);
	MAG_lock = xSemaphoreCreateMutex();

	uint8_t address[1] = {10};
	uint8_t id[3];

	I2C_M_SETUP_Type s;
	s.sl_addr7bit = HMC5843_ADDRESS;
	s.tx_data = address;
	s.tx_length = 1;
	s.rx_data = id;
	s.rx_length = 3;
	s.retransmissions_max = 3;
	I2C0_sendS(&s);

	if( !(s.status & I2C_SETUP_STATUS_DONE) || (id[0] != 'H' || id[1] != '4' || id[2] != '3')){
		xputs("Magneto is not responding!\n");
		return;
	}

	uint8_t data[4] = {0,
			0b00011000,	//A = 50Hz Output Rate
			0b01100000,	//B = +- 2.0 Gauss
			0b00000000};//Mode = Continuous Conversion

	s.sl_addr7bit = HMC5843_ADDRESS;
	s.tx_data = data;
	s.tx_length = 4;
	s.rx_data = NULL;
	s.rx_length = 0;
	s.retransmissions_max = 3;
	I2C0_sendS(&s);

	if( !(s.status & I2C_SETUP_STATUS_DONE)){
		xputs("Magneto failed to respond to settings update!");
		return;
	}

	//TODO do a self test
	//TODO calculate bias
	//TODO compensate for temperature

	offset[0] = MAGN_X_OFFSET;
	offset[1] = MAGN_Y_OFFSET;
	offset[2] = MAGN_Z_OFFSET;

	Magneto_Update();
}

double Magneto_GetHeading(){
	double h = 0;

	if(xSemaphoreTake(MAG_lock, portMAX_DELAY) == pdTRUE){
		h = heading;
		xSemaphoreGive(MAG_lock);
	}

	return h;
}

double Magneto_Get(uint8_t axis){
	return scaled[axis];
}

int16_t Magneto_GetRaw(uint8_t axis){
	return raw[axis];
}

double Magneto_GetX(){
	return scaled[X];
}

double Magneto_GetY(){
	return scaled[Y];
}

double Magneto_GetZ(){
	return scaled[Z];
}

