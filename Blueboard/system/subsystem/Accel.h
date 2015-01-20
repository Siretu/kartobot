/*
 * Accelerometer.h
 *
 *  Created on: 28 Dec 2011
 *      Author: exuvo
 */

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_
#ifdef __cplusplus
extern "C" {
#endif

#define ACCEL_X_MIN (-276.0)
#define ACCEL_X_MAX ( 249.0)
#define ACCEL_Y_MIN (-282.0)
#define ACCEL_Y_MAX ( 252.0)
#define ACCEL_Z_MIN (-262.0)
#define ACCEL_Z_MAX ( 254.0)

#define GRAVITY 256.0f // "1G reference" used for DCM filter and accelerometer calibration

#define ACCEL_X_OFFSET ((ACCEL_X_MIN + ACCEL_X_MAX) / 2.0)
#define ACCEL_Y_OFFSET ((ACCEL_Y_MIN + ACCEL_Y_MAX) / 2.0)
#define ACCEL_Z_OFFSET ((ACCEL_Z_MIN + ACCEL_Z_MAX) / 2.0)
#define ACCEL_X_SCALE (GRAVITY / (ACCEL_X_MAX - ACCEL_X_OFFSET))
#define ACCEL_Y_SCALE (GRAVITY / (ACCEL_Y_MAX - ACCEL_Y_OFFSET))
#define ACCEL_Z_SCALE (GRAVITY / (ACCEL_Z_MAX - ACCEL_Z_OFFSET))

double Accel_Get(uint8_t axis);
int16_t Accel_GetRaw(uint8_t axis);
double Accel_GetX();
double Accel_GetY();
double Accel_GetZ();
double Accel_OneG();
void Accel_Update();
void Accel_Init();

#ifdef __cplusplus
}
#endif
#endif /* ACCELEROMETER_H_ */
