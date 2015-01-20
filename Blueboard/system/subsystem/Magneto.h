/*
 * Magnetometer.h
 *
 *  Created on: 28 Dec 2011
 *      Author: exuvo
 */

#ifndef MAGNETOMETER_H_
#define MAGNETOMETER_H_
#ifdef __cplusplus
extern "C" {
#endif

#define MAGN_X_MIN ( 1138.0)
#define MAGN_X_MAX ( 1847.0)
#define MAGN_Y_MIN (-1835.0)
#define MAGN_Y_MAX (-1121.0)
#define MAGN_Z_MIN ( 907.0)
#define MAGN_Z_MAX ( 1557.0)

#define MAGN_X_OFFSET ((MAGN_X_MIN + MAGN_X_MAX) / 2.0)
#define MAGN_Y_OFFSET ((MAGN_Y_MIN + MAGN_Y_MAX) / 2.0)
#define MAGN_Z_OFFSET ((MAGN_Z_MIN + MAGN_Z_MAX) / 2.0)
#define MAGN_X_SCALE (100.0f / (MAGN_X_MAX - MAGN_X_OFFSET))
#define MAGN_Y_SCALE (100.0f / (MAGN_Y_MAX - MAGN_Y_OFFSET))
#define MAGN_Z_SCALE (100.0f / (MAGN_Z_MAX - MAGN_Z_OFFSET))

double Magneto_Get(uint8_t axis);
int16_t Magneto_GetRaw(uint8_t axis);
double Magneto_GetX();
double Magneto_GetY();
double Magneto_GetZ();
double Magneto_GetHeading();
void Magneto_Update();
void Magneto_Init();

#ifdef __cplusplus
}
#endif
#endif /* MAGNETOMETER_H_ */
