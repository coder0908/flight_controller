/*
 * pwm_app.c
 *
 *  Created on: May 3, 2025
 *      Author: mia00
 */

#include "consur.h"
#include "pp.h"

struct servo  bldc_left_mot, bldc_right_mot, servo_left_ail, servo_right_ail, servo_ele, servo_rud, servo_lan, servo_left_flap, servo_right_flap;

bool consur_init()
{
	bool ret = false;

	servo_init(&bldc_left_mot, &htim1, TIM_CHANNEL_1, 174, 1811, 500, 1000);
	servo_init(&bldc_right_mot, &htim1, TIM_CHANNEL_2, 174, 1811, 500, 1000);
	servo_init(&servo_left_ail, &htim3, TIM_CHANNEL_4, 0, 1000, 500, 1000);
	servo_init(&servo_right_ail, &htim4, TIM_CHANNEL_2, 0, 1000, 500, 1000);
	servo_init(&servo_ele, &htim3, TIM_CHANNEL_2, 0, 1000, 250, 1250);
	servo_init(&servo_rud, &htim4, TIM_CHANNEL_1, 174, 1811, 250, 1250);
	servo_init(&servo_lan, &htim3, TIM_CHANNEL_1, 174, 1811, 250, 1250);
	servo_init(&servo_left_flap, &htim2, TIM_CHANNEL_2, 0, 1000, 250, 1250);
	servo_init(&servo_right_flap, &htim3, TIM_CHANNEL_3, 0, 1000, 250, 1250);

	ret = servo_write(&servo_left_ail, 500);
	if (!ret) {
		return false;
	}
	ret = servo_write(&servo_right_ail, 500);
	if (!ret) {
		return false;
	}
	ret = servo_write(&servo_ele, 750);
	if (!ret) {
		return false;
	}
	ret = servo_write(&servo_rud, 500);
	if (!ret) {
		return false;
	}
	ret = servo_write(&bldc_left_mot, 174);
	if (!ret) {
		return false;
	}
	ret = servo_write(&bldc_right_mot, 174);
	if (!ret) {
		return false;
	}
	ret = servo_write(&servo_lan, 500);
	if (!ret) {
		return false;
	}
	ret = servo_write(&servo_left_flap, 500);
	if (!ret) {
		return false;
	}
	ret = servo_write(&servo_right_flap, 500);
	if (!ret) {
		return false;
	}

	return ret;
}


