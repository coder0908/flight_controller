/*
 * pwm_app.c
 *
 *  Created on: May 3, 2025
 *      Author: coder0908
 */

#include "platform/hal/platform_hal.h"
#include "drivers/servo_driver/servo_driver.h"
#include "consur.h"

struct servo  bldc_left_mot, bldc_right_mot, servo_left_ail, servo_right_ail, servo_ele, servo_rud, servo_lan, servo_left_flap, servo_right_flap;

bool consur_init()
{
	bool ret = false;

	servo_init(&bldc_left_mot, &htim1, TIM_CHANNEL_1, 174, 1811, 500, 1000);
	servo_init(&bldc_right_mot, &htim1, TIM_CHANNEL_2, 174, 1811, 500, 1000);
	servo_init(&servo_left_ail, &htim3, TIM_CHANNEL_4, 174, 1811, 500, 1000);
	servo_init(&servo_right_ail, &htim4, TIM_CHANNEL_2, 174, 1811, 500, 1000);
	servo_init(&servo_ele, &htim3, TIM_CHANNEL_2, 174, 1811, 250, 1250);
	servo_init(&servo_rud, &htim4, TIM_CHANNEL_1, 174, 1811, 250, 1250);
	servo_init(&servo_lan, &htim3, TIM_CHANNEL_1, 174, 1811, 250, 1250);
	servo_init(&servo_left_flap, &htim2, TIM_CHANNEL_2, 174, 1811, 250, 1250);
	servo_init(&servo_right_flap, &htim3, TIM_CHANNEL_3, 174, 1811, 250, 1250);

	ret = servo_write(&servo_left_ail, 922);
	if (!ret) {
		return false;
	}
	ret = servo_write(&servo_right_ail, 922);
	if (!ret) {
		return false;
	}
	ret = servo_write(&servo_ele, 922);
	if (!ret) {
		return false;
	}
	ret = servo_write(&servo_rud, 992);
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
	ret = servo_write(&servo_lan, 922);
	if (!ret) {
		return false;
	}
	ret = servo_write(&servo_left_flap, 922);
	if (!ret) {
		return false;
	}
	ret = servo_write(&servo_right_flap, 922);
	if (!ret) {
		return false;
	}

	return ret;
}


