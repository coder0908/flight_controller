	/*
 * pid.h
 *
 *  Created on: Nov 19, 2025
 *      Author: unRR
 */

#ifndef LIB_PID_PID_H_
#define LIB_PID_PID_H_

#include <stdbool.h>
#include <stdint.h>

struct single_pid {
	float kp;
	float ki;
	float kd;

	float error_sum;
	float prev_state;

	float d_filter_gain;

	float error_sum_min;
	float error_sum_max;

	uint32_t prev_time;

	float p_result;
	float i_result;
	float d_result;

	float pid_result;
};

struct cascade_pid {
	struct single_pid inner;
	struct single_pid outter;
};
bool cascade_pid_init(struct cascade_pid *cpid, float outter_kp, float outter_ki, float outter_kd, float outter_d_filter_gain, float outter_error_sum_min, float outter_error_sum_max,
		float inner_kp, float inner_ki, float inner_kd, float inner_d_filter_gain, float inner_error_sum_min, float inner_error_sum_max);
bool cascade_pid_calc(struct cascade_pid *cpid, float set_state/*ouuter*/, float outter_current_state, float inner_current_state);
#endif /* LIB_PID_PID_H_ */
