/*
 * battery.c
 *
 *  Created on: Nov 19, 2025
 *      Author: unRR
 */


#include "init/batt/batt.h"
#include "pp.h"
#include "vmd.h"

struct battery g_batt_1;
struct battery g_batt_2;

bool batt_init()
{
	bool tmp;

	tmp = battery_init(&g_batt_1, &hadc1, ADC_CHANNEL_4, ADC_SAMPLETIME_480CYCLES, (47.0f+10.0f)/10.0f);
	if (!tmp) {
		return false;
	}

	tmp = battery_init(&g_batt_2, &hadc1, ADC_CHANNEL_7, ADC_SAMPLETIME_480CYCLES, (47.0f+10.0f)/10.0f);
	if (!tmp) {
		return false;
	}

	return true;
}
