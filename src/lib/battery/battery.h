/*
 * battery.h
 *
 *  Created on: Nov 19, 2025
 *      Author: unRR
 */

#ifndef LIB_BATTERY_BATTERY_H_
#define LIB_BATTERY_BATTERY_H_

#include "pp.h"
#include <stdbool.h>
#include <stdint.h>


struct battery {
	ADC_HandleTypeDef *hadc;
	uint32_t channel;
	uint32_t sampling_time;

	float voltage_scale;	//분압비
	uint32_t adc_max_lsb;
};

bool battery_init(struct battery *bat, ADC_HandleTypeDef *hadc, uint32_t channel, uint32_t sampling_time, float voltage_scale);
bool battery_read_voltage(struct battery *bat, float *voltage);

#endif /* LIB_BATTERY_BATTERY_H_ */
