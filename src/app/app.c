/*
 * app.c
 *
 *  Created on: May 5, 2025
 *      Author: mia00
 */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "app.h"

#define TTTT (10000.0)

int _write(int file, char *p, int len)
{
	HAL_UART_Transmit(&huart1, (uint8_t*)p, len, HAL_MAX_DELAY);
	return len;
}

//static int32_t map(int32_t x, int32_t min_in, int32_t max_in, int32_t min_out, int32_t max_out)
//{
//  return (x - min_in) * (max_out - min_out) / (max_in - min_in) + min_out;
//}

bool setup()
{

	bool ret = false;

	ret = battery_init();
	if (!ret) {
		return false;
	}
//
//	ret = attitude_init();
//	if (!ret) {
//		return false;
//	}

	ret = consur_init();
	if (!ret) {
		return false;
	}

	ret = imu_init();
	if (!ret) {
		return false;
	}

	return ret;
}

//static int16_t g_x_gyro_lsb, g_y_gyro_lsb, g_z_gyro_lsb;
//static int16_t g_x_accel_lsb, g_y_accel_lsb, g_z_accel_lsb;


//static bool is_imu_data_ready = false;

void loop(void)
{
//	is_imu_data_ready = icm20602_is_data_ready(&g_imu);
//	if (is_imu_data_ready) {
//
//		icm20602_get_accel_gyro_lsb(&g_imu, &g_x_accel_lsb, &g_y_accel_lsb, &g_z_accel_lsb, &g_x_gyro_lsb, &g_y_gyro_lsb, &g_z_gyro_lsb);
//
//		icm20602_calc_angle_compfilter(&g_imu, g_x_accel_lsb, g_y_accel_lsb, g_z_accel_lsb, g_x_gyro_lsb, g_y_gyro_lsb, g_z_gyro_lsb);
//
//
//	}

//	if (is_trcivr_data_ready) {
//		if (crsf_parse_rc_channels(&g_trcivr_elrs, &g_rc_channels)) {
//			if (g_rc_channels.chan8 == 191) {
//				servo_write(&bldc_left_mot, g_rc_channels.chan1);
//
//			}
//
//			servo_write(&servo_rud, g_rc_channels.chan4);
//			servo_write(&servo_lan, g_rc_channels.chan4);
//			servo_write(&servo_ele, g_rc_channels.chan3);
//			servo_write(&servo_left_ail, g_rc_channels.chan2);
//			servo_write(&servo_right_ail, g_rc_channels.chan2);
//
//
//		}
//		crsf_flush_rx(&g_trcivr_elrs);
//	}
//
//	if (battery_read_voltage(&g_batt, batts)) {
//		crsf_send_battery(&g_trcivr_elrs, (int16_t)batts[0]*10, 0,0,0);
//		crsf_send_battery(&g_trcivr_nrf24, (int16_t)batts[0]*10, 0,0,0);
//		crsf_send_battery(&g_trcivr_nrf24, (int16_t)batts[1]*10, 0,0,0);
//
//	}

	battery_read_voltage();


}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
//	if (huart->Instance == g_trcivr_elrs.huart->Instance) {
//		HAL_UART_AbortReceive(huart);
//		__HAL_UART_CLEAR_PEFLAG(huart);
//		__HAL_UART_CLEAR_FEFLAG(huart);
//		__HAL_UART_CLEAR_NEFLAG(huart);
//		__HAL_UART_CLEAR_OREFLAG(huart);
//
////		HAL_UART_Receive_DMA(huart, g_trcivr_elrs.buf, g_trcivr_elrs.buf_size);
//	}
//
//	if (huart->Instance == g_gps.huart->Instance) {
//		HAL_UART_AbortReceive(huart);
//		__HAL_UART_CLEAR_PEFLAG(huart);
//		__HAL_UART_CLEAR_FEFLAG(huart);
//		__HAL_UART_CLEAR_NEFLAG(huart);
//		__HAL_UART_CLEAR_OREFLAG(huart);
//
////		HAL_UART_Receive_DMA(huart, g_gps.buf, g_gps.buf_size);
//	}

}





