/*
 * Output.h
 *
 *  Created on: 25 Feb 2012
 *      Author: exuvo
 */

#ifndef OUTPUT_H_
#define OUTPUT_H_
#ifdef __cplusplus
extern "C" {
#endif

extern int16_t accel_min[3];
extern int16_t accel_max[3];

extern int16_t magneto_min[3];
extern int16_t magneto_max[3];

extern int32_t gyro_average[3];
extern uint32_t gyro_num_samples;

void output_calibration(int calibration_sensor);
void output_raw();
void output_debug();

#ifdef __cplusplus
}
#endif
#endif /* OUTPUT_H_ */
