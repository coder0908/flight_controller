/*
 * neom8n_ubx.c
 *
 *  Created on: Jul 12, 2025
 *      Author: mia00
 */


#include "ubx.h"
#include "vmd.h"
#include <stdlib.h>

//ubx class
#define UBX_CLASS_NAV 		(0x01)

//ubx id
#define UBX_ID_POSLLH		(0x02)
#define UBX_ID_PVT 		(0x07)

//ubx length
#define UBX_LEN_NAV_POSLLH		(28)
#define UBX_LEN_NAV_PVT		(92)


const uint8_t UBX_CFG_PRT[28] = {
		0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00,
		0x00, 0x00, 0xD0, 0x08, 0x00, 0x00, 0x80, 0x25,
		0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x9A, 0x79
};

//NAV POSLLH
const uint8_t UBX_CFG_MSG_NAV_POSLLH[16] = {
		0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x01, 0x02,
		0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x13, 0xBE
};

//NAV PVT
const uint8_t UBX_CFG_MSG_NAV_PVT[16] = {
		0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x01, 0x07,
		0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x18, 0xE1
};


const uint8_t UBX_CFG_RATE[14] = {
		0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xC8, 0x00,
		0x01, 0x00, 0x01, 0x00, 0xDE, 0x6A
};

const uint8_t UBX_CFG_CFG[21] = {
		0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0x00, 0x00,
		0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x17, 0x31, 0xBF
};

static uint32_t buf_to_u32_little_endian(const uint8_t *buf)
{
	return buf[0] | (buf[1]<<8) | (buf[2]<<16) | (buf[3]<<24);
}

//sizeof(buf) == 3
//ignore highest 8bit
static uint32_t buf_to_u24_little_endian(const uint8_t *buf)
{
	return buf[0] | (buf[1]<<8) | (buf[2]<<16);
}

static uint16_t buf_to_u16_little_endian(const uint8_t *buf)
{
	return buf[0] | (buf[1]<<8);
}



bool transmit_ubx_data(struct ubx_gps *gps, const uint8_t *data, uint32_t size)
{
	VMD_ASSERT_PARAM(gps);
	VMD_ASSERT_PARAM(data);

	HAL_StatusTypeDef status = HAL_UART_Transmit(gps->huart, data, size, 1000);
	HAL_Delay(100);
	if (status != HAL_OK) {
		return false;
	}
	return true;
}

//without SYNC CHARS. {class, id, length, pld}
//ck_a, ck_b is nullable
bool calc_ubx_checksum(const struct ubx_gps *gps, uint8_t *ck_a, uint8_t *ck_b)
{
	VMD_ASSERT_PARAM(gps);

	uint8_t tmp_ck_a = 0;
	uint8_t tmp_ck_b = 0;

	for (uint16_t i=2; i < gps->packet_buf_size-2; i++) {
		tmp_ck_a = tmp_ck_a + gps->packet_buf[i];
		tmp_ck_b = tmp_ck_b + tmp_ck_a;
	}

	if (ck_a) {
		*ck_a = tmp_ck_a;
	}
	if (ck_b) {
		*ck_b = tmp_ck_b;
	}

	if (tmp_ck_a != gps->packet_buf[gps->packet_buf_size-2] || tmp_ck_b != gps->packet_buf[gps->packet_buf_size-1]) {
		return false;
	}
	return true;
}


bool init_ubx_gps(struct ubx_gps *gps, UART_HandleTypeDef *huart, uint8_t *packet_buf, uint32_t packet_buf_size, uint8_t *uart_rx_buf, uint32_t uart_rx_buf_size)
{
	VMD_ASSERT_PARAM(gps);
	VMD_ASSERT_PARAM(huart);
	VMD_ASSERT_PARAM(packet_buf);
	VMD_ASSERT_PARAM(uart_rx_buf);

	gps->huart = huart;

	gps->packet_buf = packet_buf;
	gps->packet_buf_size = packet_buf_size;
	gps->packet_buf_cnt = 0;

	gps->uart_rx_buf = uart_rx_buf;
	gps->uart_rx_buf_size = uart_rx_buf_size;
	gps->uart_rx_buf_cnt = 0;

	return true;
}

bool begin_ubx_gps(struct ubx_gps *gps)
{
	HAL_StatusTypeDef status = HAL_UART_Receive_IT(gps->huart, gps->uart_rx_buf, gps->uart_rx_buf_size);
	if (status != HAL_OK) {
		return false;
	}
	return true;
}

bool parse_ubx_nav_posllh(const struct ubx_gps *gps, struct ubx_nav_posllh *nav_posllh)
{
	VMD_ASSERT_PARAM(gps);
	VMD_ASSERT_PARAM(nav_posllh);

	const uint8_t *buf = gps->packet_buf;

	if (buf[2] != UBX_CLASS_NAV || buf[3] != UBX_ID_POSLLH) {
		return false;
	}
	if (buf[4] != UBX_LEN_NAV_POSLLH) {
		return false;
	}

	nav_posllh->tow_ms = buf_to_u32_little_endian(buf+6);
	nav_posllh->longitude_100ndeg =  buf_to_u32_little_endian(buf+10);
	nav_posllh->latitude_100ndeg =  buf_to_u32_little_endian(buf+14);
	nav_posllh->height_ellipsoid_mm =  buf_to_u32_little_endian(buf+18);
	nav_posllh->height_msl_mm = buf_to_u32_little_endian(buf+22);
	nav_posllh->horizontal_accuracy_mm =  buf_to_u32_little_endian(buf+26);
	nav_posllh->vertical_accuracy_mm =  buf_to_u32_little_endian(buf+30);

//	navposllh->longitude_deg = navposllh->raw_longitude / 10000000.0;
//	navposllh->latitude_deg = navposllh->raw_latitude / 10000000.0;

	return true;
}

bool parse_ubx_nav_pvt(const struct ubx_gps *gps, struct ubx_nav_pvt *pvt)
{
	VMD_ASSERT_PARAM(gps);
	VMD_ASSERT_PARAM(pvt);

	const uint8_t *buf = gps->packet_buf;

	if (buf[2] != UBX_CLASS_NAV || buf[3] != UBX_ID_PVT) {
		return false;
	}
	if (buf[4] != UBX_LEN_NAV_PVT) {
		return false;
	}

	pvt->tow_ms = buf_to_u32_little_endian(buf+6);
	pvt->year = buf_to_u16_little_endian(buf+10);
	pvt->month = buf[12];
	pvt->day = buf[13];
	pvt->hour = buf[14];
	pvt->minuate = buf[15];
	pvt->second = buf[16];
	pvt->valid = buf[17];
	pvt->time_accuracy_ns = buf_to_u32_little_endian(buf+18);
	// TODO : 마무리
	return true;
}

__weak bool ubx_callback_callback()
{
	return true;
}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) 안에서 호츌
void ubx_uart_rx_cplt_callback(UART_HandleTypeDef *huart, struct ubx_gps *gps)
{
	if (gps->huart == huart) {
		if (gps->packet_buf_cnt != gps->packet_buf_size) {
			gps->packet_buf_cnt = 0;
			gps->uart_rx_buf_cnt = 0;
			HAL_UART_Receive_IT(gps->huart, gps->uart_rx_buf, gps->packet_buf_size - gps->packet_buf_cnt);
		}
		gps->packet_buf_cnt = 0;
		gps->uart_rx_buf_cnt = 0;

		if (!ubx_callback_callback()) {
			return;
		}

		HAL_UART_Receive_IT(gps->huart, gps->packet_buf, gps->packet_buf_size);
	}
}
