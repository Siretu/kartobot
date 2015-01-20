/* This file is part of the Razor AHRS Firmware */

#include "FreeRTOS.h"
#include "task.h"
#include "term_io.h"
#include "utils.h"
#include "Output.h"
#include <stdio.h>
#include "Accel.h"
#include "Gyro.h"
#include "Magneto.h"

int16_t accel_min[3];
int16_t accel_max[3];

int16_t magneto_min[3];
int16_t magneto_max[3];

int32_t gyro_average[3];
uint32_t gyro_num_samples = 0;

void output_calibration(int calibration_sensor){
	char b[128];
	char* c = b;

	if (calibration_sensor == 0){  // Accelerometer
		c+=sprintf(c, "accel x,y,z (min/max) = ");
		for (uint8_t i = 0; i < 3; i++) {
			c+=sprintf(c, "%+05d/%+05d", accel_min[i], accel_max[i]);

			if (i < 2){
				c+=sprintf(c, "  ");
			} else {
				c+=sprintf(c, "\n");
			}
		}

	}else if (calibration_sensor == 1){  // Magneto
		c+=sprintf(c, "magn x,y,z (min/max) = ");
		for (uint8_t i = 0; i < 3; i++) {
			c+=sprintf(c, "%+06d/%+06d", magneto_min[i], magneto_max[i]);

			if (i < 2){
				c+=sprintf(c, "  ");
			} else {
				c+=sprintf(c, "\n");
			}
		}
	}else if (calibration_sensor == 2){  // Gyroscope
		if(gyro_num_samples == 0) return;
		c+=sprintf(c, "gyro x,y,z (current/average) = ");
		for (uint8_t i = 0; i < 3; i++) {
			c+=sprintf(c, "%+05d/%+08.2f", Gyro_GetRaw(i), (double) gyro_average[i] / (double) gyro_num_samples);

			if (i < 2){
				c+=sprintf(c, "  ");
			} else {
				c+=sprintf(c, "\n");
			}
		}
	}

	xputs(b);
	if(c > b + sizeof(b)){
		xputs("Overflow in output!");
	}
}

void output_raw(){
	char b[128];//25*3+1
	char* c = b;
	c+=sprintf(c, "ACC %+.4d, %+.4d, %+.4d, \n", Accel_GetRaw(0), Accel_GetRaw(1), Accel_GetRaw(2));
	c+=sprintf(c, "MAG %+.5d, %+.5d, %+.5d, \n", Magneto_GetRaw(0), Magneto_GetRaw(1), Magneto_GetRaw(2));
	c+=sprintf(c, "GYR %+.4d, %+.4d, %+.4d, \n", Gyro_GetRaw(0), Gyro_GetRaw(1), Gyro_GetRaw(2));
	xputs(b);
	if(c > b + sizeof(b)){
		xputs("Overflow in output!");
	}
}

void output_debug(){
	char b[128];//25*3+1
	char* c = b;
	c+=sprintf(c, "ACC %+08.2f, %+08.2f, %+08.2f \n", Accel_Get(0), Accel_Get(1), Accel_Get(2));
	c+=sprintf(c, "MAG %+08.2f, %+08.2f, %+08.2f \n", Magneto_Get(0), Magneto_Get(1), Magneto_Get(2));
	c+=sprintf(c, "GYR %+08.2f, %+08.2f, %+08.2f \n", Gyro_Get(0), Gyro_Get(1), Gyro_Get(2));
	xputs(b);
	if(c > b + sizeof(b)){
		xputs("Overflow in output!");
	}
}

