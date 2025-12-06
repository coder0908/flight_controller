/*
 * consur.h
 *
 *  Created on: May 5, 2025
 *      Author: coder0908
 */


#include <stdint.h>
#include <stdbool.h>

extern struct servo  bldc_left_mot, bldc_right_mot, servo_left_ail, servo_right_ail, servo_ele, servo_rud, servo_lan, servo_left_flap, servo_right_flap;

bool consur_init();

