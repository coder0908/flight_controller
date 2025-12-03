/*
 * battery.c
 *
 *  Created on: Nov 19, 2025
 *      Author: coder0908
 */


#include "pp.h"
#include "app/battery/battery.h"

#define ADC1_FULLSCALE				(4095.0f)
#define BATTERY_VOLTAGESCALE_ADC1CH4		((47.0f+10.0f)/10.0f)
#define BATTERY_VOLTAGESCALE_ADC1CH7		((47.0f+10.0f)/10.0f)
#define BATTERY_CHANNELCNT_ADC1			2



static uint32_t adc1_values_lsb[BATTERY_CHANNELCNT_ADC1 + 1];	// last one's for vrefint

static float battery_adc1_voltages_v[BATTERY_CHANNELCNT_ADC1];

bool battery_init()
{
	HAL_StatusTypeDef status;

	status = HAL_ADC_Start_DMA(&hadc1, adc1_values_lsb, BATTERY_CHANNELCNT_ADC1+1);	//+1 for vrefint channel
	if (status != HAL_OK) {
		return false;
	}

	return true;
}

void battery_read_voltage()
{
	float vrefplus_mv = (float)VREFINT_CAL_VREF * (float)(*VREFINT_CAL_ADDR) / (float)adc1_values_lsb[BATTERY_CHANNELCNT_ADC1];	// battery_adc1_values[BATTERY_CHANNELCNT_ADC1] = vrefint data

	battery_adc1_voltages_v[0] = ((float)adc1_values_lsb[0]/ADC1_FULLSCALE) * (vrefplus_mv/1000.0f) * BATTERY_VOLTAGESCALE_ADC1CH4;
	battery_adc1_voltages_v[1] = ((float)adc1_values_lsb[1]/ADC1_FULLSCALE) * (vrefplus_mv/1000.0f) * BATTERY_VOLTAGESCALE_ADC1CH7;

}



