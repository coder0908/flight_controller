/*
 * mahony.h
 *
 *  Created on: Nov 20, 2025
 *      Author: unRR
 */

#ifndef LIB_MAHONY_MAHONY_H_
#define LIB_MAHONY_MAHONY_H_

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

struct mahony_ahrs {
	float q0, q1, q2, q3;

	float x_gyro_bias_radps;
	float y_gyro_bias_radps;
	float z_gyro_bias_radps;

	float kp;
	float ki;

	uint32_t prev_time;
};


bool mahony_init(struct mahony_ahrs *mah, float kp, float ki);
bool mahony_calc(struct mahony_ahrs *mah,
		float x_gyro_radps, float y_gyro_radps, float z_gyro_radps,
		float x_accel_g, float y_accel_g, float z_accel_g,
		float x_mag, float y_mag, float z_mag);
bool mahony_convert_euler_angle(const struct mahony_ahrs *mah, float *x_angle_rad, float *y_angle_rad, float *z_angle_rad);

#endif /* LIB_MAHONY_MAHONY_H_ */
