/*
 * Razor_AHRS.h
 *
 *  Created on: 25 Feb 2012
 *      Author: exuvo
 */

#ifndef RAZOR_AHRS_H_
#define RAZOR_AHRS_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "task.h"

// When defined, gyro drift correction will not be applied
//#define DEBUG__NO_DRIFT_CORRECTION

#define MODE_SENSORS_NORMAL 0
#define MODE_CALIBRATE_SENSORS 1 // Outputs sensor min/max values as text for manual calibration
#define MODE_SENSORS_TEXT 2 // Outputs (calibrated) sensor values for all 9 axes as text

extern float G_Dt;

double AHRS_Get(uint8_t axis);
double AHRS_GetRoll();
double AHRS_GetPitch();
double AHRS_GetYaw();
void AHRS_NextSensor();
void AHRS_ModeCalibrate();
void AHRS_ModeDebug();
void AHRS_ModeNormal();
void AHRS_Init();
void AHRS_Run(void *pvParameters);
extern const xTaskParameters ahrsTaskDefinition;

#ifdef __cplusplus
}
#endif
#endif /* RAZOR_AHRS_H_ */
