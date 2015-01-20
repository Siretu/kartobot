/*
 * Math.h
 *
 *  Created on: 25 Feb 2012
 *      Author: exuvo
 */

#ifndef MATH_H_
#define MATH_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <Math.h>

float Vector_Dot_Product(float vector1[3], float vector2[3]);
void Vector_Cross_Product(float vectorOut[3], float v1[3], float v2[3]);
void Vector_Scale(float vectorOut[3], float vectorIn[3], float scale2);
void Vector_Add(float vectorOut[3], float vectorIn1[3], float vectorIn2[3]);
void Matrix_Multiply(float a[3][3], float b[3][3],float mat[3][3]);
void init_rotation_matrix(float m[3][3], float yaw, float pitch, float roll);

#ifdef __cplusplus
}
#endif
#endif /* MATH_H_ */
