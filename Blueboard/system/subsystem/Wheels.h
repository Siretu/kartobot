/*
 * QuadratureEncoder.h
 *
 *  Created on: 21 Nov 2011
 *      Author: exuvo
 */

#ifndef WHEELS_H_
#define WHEELS_H_
#ifdef __cplusplus
extern "C" {
#endif

#define MOTOR_FL 0
#define MOTOR_FR 1
#define MOTOR_RL 2
#define MOTOR_RR 3

#define WHEEL_DIAMATER 6.2

void Wheels_Init();
void Wheels_Reset();
int32_t Wheels_GetPulses(uint8_t motor);
int32_t Wheels_GetRevolutions(uint8_t motor);
int32_t Wheels_GetDistance(uint8_t motor);//Centimeters
double Wheels_GetDistanceD(uint8_t motor);//Centimeters

#ifdef __cplusplus
}
#endif
#endif /* WHEELS_H_ */
