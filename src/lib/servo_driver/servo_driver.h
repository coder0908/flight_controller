/*
 * servo.h
 *
 *  Created on: Jun 28, 2025
 *      Author: mia00
 */

#ifndef __SERVO_DRIVER_H__
#define __SERVO_DRIVER_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

struct servo {
	TIM_HandleTypeDef *htim;
	uint32_t channel;

	uint32_t min_input;
	uint32_t max_input;

	uint32_t min_ccr;
	uint32_t max_ccr;
};

bool servo_init(struct servo *servo, TIM_HandleTypeDef *htim, uint32_t channel, uint32_t min_input, uint32_t max_input, uint32_t min_ccr, uint32_t max_ccr);
bool servo_write(struct servo *servo, uint32_t input);

#endif /* __SERVO_DRIVER_H__ */
