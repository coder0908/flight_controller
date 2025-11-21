/*
 * battery.c
 *
 *  Created on: Nov 19, 2025
 *      Author: unRR
 */


#include "battery.h"
#include "vmd.h"

static bool battery_select_channel(struct battery *bat)
{
	VMD_ASSERT_PARAM(bat);

	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = bat->channel;
	sConfig.Rank = 1;
	sConfig.SamplingTime = bat->sampling_time;
	if (HAL_ADC_ConfigChannel(bat->hadc, &sConfig) != HAL_OK)
	{
		return false;
	}

	return true;
}

static bool battery_read_adc(struct battery *bat, uint32_t *adc)
{
	VMD_ASSERT_PARAM(bat);
	VMD_ASSERT_PARAM(adc);

	HAL_StatusTypeDef status;
	bool tmp;

	tmp = battery_select_channel(bat);
	if (!tmp) {
		return false;
	}

	status = HAL_ADC_Start(bat->hadc);
	if (status != HAL_OK) {
		return false;
	}
	status = HAL_ADC_PollForConversion(bat->hadc, 100);
	if (status != HAL_OK) {
		return false;
	}
	*adc = HAL_ADC_GetValue(bat->hadc);
	status = HAL_ADC_Stop(bat->hadc);
	if (status != HAL_OK) {
		return false;
	}

	return true;
}

static bool battery_read_vdda(struct battery *bat, float *vdda)
{
	VMD_ASSERT_PARAM(bat);
	VMD_ASSERT_PARAM(vdda);

	HAL_StatusTypeDef status;
	bool tmp;

	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = ADC_CHANNEL_VREFINT;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	if (HAL_ADC_ConfigChannel(bat->hadc, &sConfig) != HAL_OK)
	{
		return false;
	}

	status = HAL_ADC_Start(bat->hadc);
	if (status != HAL_OK) {
		return false;
	}
	status = HAL_ADC_PollForConversion(bat->hadc, 100);
	if (status != HAL_OK) {
		return false;
	}
	uint32_t vrefint = HAL_ADC_GetValue(bat->hadc);
	status = HAL_ADC_Stop(bat->hadc);
	if (status != HAL_OK) {
		return false;
	}

	uint16_t *vrefint_cal = (uint16_t*)0x1fff7a2a;	//TODO : uint16, uint32_t 뭔지 알아내기

	*vdda = (3.3 * (*vrefint_cal)) / vrefint;

	return true;

}


bool battery_init(struct battery *bat, ADC_HandleTypeDef *hadc, uint32_t channel, uint32_t sampling_time, float voltage_scale)
{
	VMD_ASSERT_PARAM(bat);
	VMD_ASSERT_PARAM(hadc);

	bat->hadc = hadc;
	bat->channel = channel;
	bat->sampling_time = sampling_time;
	bat->voltage_scale = voltage_scale;

	switch (bat->hadc->Init.Resolution) {
	case ADC_RESOLUTION_12B:
		bat->adc_max_lsb = 4095;
		break;
	case ADC_RESOLUTION_10B:
		bat->adc_max_lsb = 1023;
		break;
	case ADC_RESOLUTION_8B:
		bat->adc_max_lsb = 255;
		break;
	case ADC_RESOLUTION_6B:
		bat->adc_max_lsb = 63;
		break;
	default:
		return false;

	}

	return true;
}

bool battery_read_voltage(struct battery *bat, float *voltage)
{
	VMD_ASSERT_PARAM(bat);
	VMD_ASSERT_PARAM(voltage);

	bool tmp;
	float vdda;
	uint32_t adc;

	tmp = battery_read_vdda(bat, &vdda);
	if (!tmp) {
		return false;
	}
	tmp = battery_read_adc(bat, &adc);
	if (!tmp) {
		return false;
	}

	*voltage = ((float)adc/(float)bat->adc_max_lsb) * vdda * bat->voltage_scale;

	return true;

}








