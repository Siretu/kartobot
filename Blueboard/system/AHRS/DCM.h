/*
 * DCM.h
 *
 *  Created on: 25 Feb 2012
 *      Author: exuvo
 */

#ifndef DCM_H_
#define DCM_H_
#ifdef __cplusplus
extern "C" {
#endif

// DCM parameters
#define Kp_ROLLPITCH 0.02
#define Ki_ROLLPITCH 0.00002
#define Kp_YAW 1.2
#define Ki_YAW 0.00002

// DCM variables
//extern float MAG_Heading;
//extern float Accel_Vector[3];
//extern float Gyro_Vector[3];
//extern float Omega_Vector[3];
//extern float Omega_P[3];
//extern float Omega_I[3];
//extern float Omega[3];
//extern float errorRollPitch[3];
//extern float errorYaw[3];
extern float DCM_Matrix[3][3];
//extern float Update_Matrix[3][3];
//extern float Temporary_Matrix[3][3];

// Euler angles
extern float yaw;
extern float pitch;
extern float roll;

void Normalize(void);
void Drift_correction(void);
void Matrix_update(void);
void Euler_angles(void);

#ifdef __cplusplus
}
#endif
#endif /* DCM_H_ */
