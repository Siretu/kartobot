/*
 * Gyro..h
 *
 *  Created on: 28 Dec 2011
 *      Author: exuvo
 */

#ifndef GYRO_H_
#define GYRO_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "lpc_types.h"

#define GYRO_AVERAGE_OFFSET_X (-7.72)
#define GYRO_AVERAGE_OFFSET_Y (-6.62)
#define GYRO_AVERAGE_OFFSET_Z ( 27.85)

// Gain for gyroscope (ITG-3200)
#define GYRO_GAIN 0.06957 // Same gain on all axes
#define GYRO_SCALED_RAD(x) ((double)(x) * TO_RAD(GYRO_GAIN)) // Calculate the scaled gyro readings in radians per second

float Gyro_GetTemp();
double Gyro_Get(uint8_t axis);
int16_t Gyro_GetRaw(uint8_t axis);
double Gyro_GetRoll();
double Gyro_GetPitch();
double Gyro_GetYaw();
void Gyro_Update();
void Gyro_Init();

#ifdef __cplusplus
}
#endif
#endif /* GYRO_H_ */
