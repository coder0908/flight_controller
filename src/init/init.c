/*
 * app.c
 *
 *  Created on: May 5, 2025
 *      Author: mia00
 */


#include "init.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "crsf/crsf.h"

#define TTTT (10000.0)

int _write(int file, char *p, int len)
{
	HAL_UART_Transmit(&huart1, (uint8_t*)p, len, HAL_MAX_DELAY);
	return len;
}

static int32_t map(int32_t x, int32_t min_in, int32_t max_in, int32_t min_out, int32_t max_out)
{
  return (x - min_in) * (max_out - min_out) / (max_in - min_in) + min_out;
}

bool setup()
{

	bool ret = false;

	ret = batt_init();
	if (!ret) {
		return false;
	}

	ret = attitude_init();
	if (!ret) {
		return false;
	}
	ret = consur_init();
	if (!ret) {
		return false;
	}

	ret = trcivr_init();
	if (!ret) {
		return false;
	}

	ret = gps_init();
	if (!ret) {
		return false;
	}

	ret = imu_init();
	if (!ret) {
		return false;
	}


	return ret;
}

static int16_t g_x_gyro_lsb, g_y_gyro_lsb, g_z_gyro_lsb;
static int16_t g_x_accel_lsb, g_y_accel_lsb, g_z_accel_lsb;
static float g_x_gyro_dps, g_y_gyro_dps, g_z_gyro_dps;
static float g_x_accel_g, g_y_accel_g, g_z_accel_g;


void loop(void)
{

	if (icm20602_is_data_ready(&g_imu)) {

		icm20602_get_accel_gyro_lsb(&g_imu, &g_x_accel_lsb, &g_y_accel_lsb, &g_z_accel_lsb, &g_x_gyro_lsb, &g_y_gyro_lsb, &g_z_gyro_lsb);

		icm20602_calc_angle_compfilter(&g_imu, g_x_accel_lsb, g_y_accel_lsb, g_z_accel_lsb, g_x_gyro_lsb, g_y_gyro_lsb, g_z_gyro_lsb);

		crsf_send_attitude(&g_trcivr, (int16_t)(DEG_TO_RAD(g_imu.x_angle_deg)*10000), (int16_t)(DEG_TO_RAD(g_imu.y_angle_deg)*10000), (int16_t)(DEG_TO_RAD(g_imu.z_angle_deg)*10000));
		//printf("%ld, %ld, %ld\n\r", (int32_t)g_imu.x_angle_deg, (int32_t)g_imu.y_angle_deg, (int32_t)g_imu.z_angle_deg);

		if (crsf_is_data_ready(&g_trcivr)) {
			if (crsf_parse_rc_channels(&g_trcivr, &g_rc_channels)) {
				float pitch_set_state = (float)map(g_rc_channels.chan3, 172, 1811, -50, 50);
				float roll_set_state = (float)map(g_rc_channels.chan2, 172, 1811, -50, 50);


				cascade_pid_calc(&g_pitch_cpid, pitch_set_state, g_imu.x_angle_deg, g_imu.x_gyro_dps);
				cascade_pid_calc(&g_roll_cpid, roll_set_state,  g_imu.y_angle_deg, g_imu.y_gyro_dps);

			//	printf("%ld, %ld\n\r", (int32_t)g_imu.x_angle_deg, (int32_t)g_pitch_cpid.inner.pid_result);

				printf("%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n\r", g_rc_channels.chan1, g_rc_channels.chan2, g_rc_channels.chan3, g_rc_channels.chan4, g_rc_channels.chan5, g_rc_channels.chan6, g_rc_channels.chan7, g_rc_channels.chan8, g_rc_channels.chan9, g_rc_channels.chan10, g_rc_channels.chan11, g_rc_channels.chan12, g_rc_channels.chan13, g_rc_channels.chan14, g_rc_channels.chan15, g_rc_channels.chan16);

				servo_write(&servo_ele, g_pitch_cpid.inner.pid_result+500);
				servo_write(&servo_left_ail, g_roll_cpid.inner.pid_result+500);
				servo_write(&servo_right_ail, g_roll_cpid.inner.pid_result+500);
				servo_write(&servo_rud, g_rc_channels.chan4);
				servo_write(&servo_lan, g_rc_channels.chan4);
				servo_write(&bldc_left_mot, g_rc_channels.chan1);

			} else {
				crsf_flush_rx(&g_trcivr);
			}
		}
	}

	float batt_1_v, batt_2_v;
	battery_read_voltage(&g_batt_1, &batt_1_v);
	battery_read_voltage(&g_batt_2, &batt_2_v);



	if (ubx_is_data_ready(&g_gps)) {
		bool tmp = ubx_parse_nav_pvt(&g_gps, &g_gps_nav_pvt);
		if (tmp) {
			crsf_send_gps(&g_trcivr, g_gps_nav_pvt.latitude_100ndeg, g_gps_nav_pvt.longitude_100ndeg, g_gps_nav_pvt.groundspeed_mmps/10000, g_gps_nav_pvt.heading_of_vehicle_10udeg*1000, g_gps_nav_pvt.alti_ellipsoid_mm/1000, g_gps_nav_pvt.satellite);
//			crsf_send_gps_ex(&g_trcivr, g_gps_nav_pvt.gnss_fix_type, g_gps_nav_pvt.northward_velocity_mmps/10, g_gps_nav_pvt.eastward_velocity_mmps/10, g_gps_nav_pvt.down_velocity_mmps/10, 0, g_gps_nav_pvt.heading_accuracy_10udeg/100000, g_gps_nav_pvt.alti_ellipsoid_mm/1000, g_gps_nav_pvt.horizontal_accuracy_mm/10, g_gps_nav_pvt.vertical_accuracy_mm/10, g_gps_nav_pvt.position_dop_centi/10, g_gps_nav_pvt.horizontal_accuracy_mm/10);
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	ubx_uart_rx_cplt_callback(huart, &g_gps);
	crsf_uart_rx_cplt_callback(huart, &g_trcivr);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if (huart == g_trcivr.huart) {
		HAL_UART_AbortReceive(huart);
		__HAL_UART_CLEAR_PEFLAG(huart);
		__HAL_UART_CLEAR_FEFLAG(huart);
		__HAL_UART_CLEAR_NEFLAG(huart);
		__HAL_UART_CLEAR_OREFLAG(huart);

		crsf_flush_rx(&g_trcivr);
		HAL_UART_Receive_DMA(huart, g_trcivr.buf, g_trcivr.buf_size);
	}

	if (huart == g_gps.huart) {
		HAL_UART_AbortReceive(huart);
		__HAL_UART_CLEAR_PEFLAG(huart);
		__HAL_UART_CLEAR_FEFLAG(huart);
		__HAL_UART_CLEAR_NEFLAG(huart);
		__HAL_UART_CLEAR_OREFLAG(huart);

		crsf_flush_rx(&g_trcivr);
		HAL_UART_Receive_DMA(huart, g_gps.buf, g_gps.buf_size);
	}

}





