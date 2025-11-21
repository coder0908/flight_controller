	/*
 * neom8n_ubx.c
 *
 *  Created on: Jul 12, 2025
 *      Author: mia00
 */


#include "ubx.h"
#include "vmd.h"
#include <stdlib.h>
#include <stdio.h>





static uint32_t buf_to_u32_little_endian(const uint8_t *buf)
{
	return buf[0] | (buf[1]<<8) | (buf[2]<<16) | (buf[3]<<24);
}

//ignore highest 8bit
static uint32_t buf_to_u24_little_endian(const uint8_t *buf)
{
	return buf[0] | (buf[1]<<8) | (buf[2]<<16);
}

static uint16_t buf_to_u16_little_endian(const uint8_t *buf)
{
	return buf[0] | (buf[1]<<8);
}



bool ubx_send_data(struct ubx_gps *gps, const uint8_t *buf, uint32_t size)
{
	VMD_ASSERT_PARAM(gps);
	VMD_ASSERT_PARAM(buf);

	HAL_StatusTypeDef status = HAL_UART_Transmit(gps->huart, buf, size, 1000);
	HAL_Delay(100);
	if (status != HAL_OK) {
		return false;
	}
	return true;
}

//without SYNC CHARS. {class, id, length, pld}
//ck_a, ck_b is nullable
bool ubx_calc_checksum(const uint8_t *buf, uint16_t size, uint8_t *ck_a, uint8_t *ck_b)
{
	VMD_ASSERT_PARAM(buf);

	uint8_t tmp_ck_a = 0;
	uint8_t tmp_ck_b = 0;

	for (uint16_t i=2; i < size-2; i++) {
		tmp_ck_a = tmp_ck_a + buf[i];
		tmp_ck_b = tmp_ck_b + tmp_ck_a;
	}

	if (ck_a) {
		*ck_a = tmp_ck_a;
	}
	if (ck_b) {
		*ck_b = tmp_ck_b;
	}

	if (tmp_ck_a != buf[size-2] || tmp_ck_b != buf[size-1]) {
		return false;
	}
	return true;
}


bool ubx_gps_init(struct ubx_gps *gps, UART_HandleTypeDef *huart)
{
	VMD_ASSERT_PARAM(gps);
	VMD_ASSERT_PARAM(huart);

	gps->huart = huart;
	gps->buf_size = UBX_BUF_SIZE;
	gps->buf_ready = false;

	return true;
}

bool ubx_gps_begin(struct ubx_gps *gps)
{
	HAL_StatusTypeDef status;
	status = HAL_UART_Receive_DMA(gps->huart, gps->buf, gps->buf_size);
	if (status != HAL_OK) {
		return false;
	}

	return true;
}

bool ubx_parse_frame(struct ubx_frame *frame, const uint8_t *buf, uint16_t size)
{
	VMD_ASSERT_PARAM(frame);
	VMD_ASSERT_PARAM(buf);

	if (size < 6) {
		return 0;
	}

	if (buf[0] != UBX_SYNC_CHAR_1 || buf[1] != UBX_SYNC_CHAR_2) {
		return false;
	}

	frame->class = buf[2];
	frame->id = buf[3];

	uint16_t *length = (uint16_t*)(buf+4);

	if (size < length[0]+UBX_SIZE_METADATA || length[0] > UBX_PLD_SIZE_MAX) {
		return false;
	}
	frame->length = length[0];

	bool is_crc_correct = ubx_calc_checksum(buf, length[0]+UBX_SIZE_METADATA, &(frame->ck_a), &(frame->ck_b));
	if (!is_crc_correct) {
		return false;
	}

	for (uint16_t i=6; i<length[0]; i++) {
		frame->payload[i-6] = buf[i];
	}

	return true;
}

bool ubx_is_data_ready(struct ubx_gps *gps)
{
	VMD_ASSERT_PARAM(gps);

	if (!gps->buf_ready) {
		return false;
	}

	gps->buf_ready = false;
	const uint8_t * const buf = gps->buf;
	const uint8_t buf_size = gps->buf_size;

	struct ubx_frame *frame;
	uint8_t tmp_byte_cnt;
	for (uint8_t byte_cnt=0; byte_cnt<buf_size; /*intentionally do nothing*/) {
		if (gps->frame_queue_cnt == UBX_LEN_FRAME_QUEUE) {
			break;
		}

		frame = gps->frame_queue + ((gps->frame_queue_head + gps->frame_queue_cnt) % UBX_LEN_FRAME_QUEUE);
		tmp_byte_cnt = ubx_parse_frame(frame, buf+byte_cnt, buf_size-byte_cnt);

		//정상적인 패킷이 잘려서 들어온 경우, 비효율.
		if (!tmp_byte_cnt) {
			byte_cnt++;
			continue;
		}

		byte_cnt += tmp_byte_cnt;
		gps->frame_queue_cnt += 1;


	}

	HAL_StatusTypeDef status;
	for (int i =0; i<5; i++) {
		status = HAL_UART_Receive_DMA(gps->huart, gps->buf, gps->buf_size);
		if (status == HAL_OK) {
			break;
		} else if(i == 4) {
			return false;
		}
	}

	if (gps->frame_queue_cnt == 0) {
		return false;
	}

	return true;
}


//flush only frame_queue not buf
bool ubx_flush_rx(struct ubx_gps *gps)
{
	VMD_ASSERT_PARAM(gps);

	gps->frame_queue_cnt = 0;

	return true;
}


bool ubx_parse_nav_posllh(struct ubx_gps *gps, struct ubx_nav_posllh *nav_posllh)
{
	VMD_ASSERT_PARAM(gps);
	VMD_ASSERT_PARAM(nav_posllh);

	if (!gps->frame_queue_cnt) {
		return false;
	}

	const struct ubx_frame *const frame = gps->frame_queue + gps->frame_queue_head;
	const uint8_t *payload = frame->payload;

	if (frame->class != UBX_CLASS_NAV || frame->id != UBX_ID_POSLLH) {
		return false;
	}

	if (frame->length != UBX_PLD_SIZE_NAV_POSLLH) {
		return false;
	}


	nav_posllh->tow_ms = buf_to_u32_little_endian(payload);
	nav_posllh->longitude_100ndeg =  buf_to_u32_little_endian(payload+4);
	nav_posllh->latitude_100ndeg =  buf_to_u32_little_endian(payload+8);
	nav_posllh->alti_ellipsoid_mm =  buf_to_u32_little_endian(payload+12);
	nav_posllh->alti_msl_mm = buf_to_u32_little_endian(payload+16);
	nav_posllh->horizontal_accuracy_mm =  buf_to_u32_little_endian(payload+20);
	nav_posllh->vertical_accuracy_mm =  buf_to_u32_little_endian(payload+24);

	gps->frame_queue_head = (gps->frame_queue_head + 1) % UBX_LEN_FRAME_QUEUE;
	gps->frame_queue_cnt -= 1;

	return true;
}

bool ubx_parse_nav_pvt(struct ubx_gps *gps, struct ubx_nav_pvt *nav_pvt)
{
	VMD_ASSERT_PARAM(gps);
	VMD_ASSERT_PARAM(nav_pvt);

	if (!gps->frame_queue_cnt) {
		return false;
	}

	const struct ubx_frame *const frame = gps->frame_queue + gps->frame_queue_head;
	const uint8_t *payload = frame->payload;

	if (frame->class != UBX_CLASS_NAV || frame->id != UBX_ID_PVT) {
		return false;
	}

	if (frame->length != UBX_PLD_SIZE_NAV_PVT) {
		return false;
	}

	nav_pvt->tow_ms = buf_to_u32_little_endian(payload);
	nav_pvt->year = buf_to_u16_little_endian(payload+4);
	nav_pvt->month = payload[6];
	nav_pvt->day = payload[12];
	nav_pvt->hour = payload[8];
	nav_pvt->minuate = payload[9];
	nav_pvt->second = payload[10];
	nav_pvt->valid = payload[11];
	nav_pvt->time_accuracy_ns = buf_to_u32_little_endian(payload+12);
	nav_pvt->fraction_of_second_ns = buf_to_u32_little_endian(payload+16);
	nav_pvt->gnss_fix_type = payload[20];
	nav_pvt->flags = payload[21];
	nav_pvt->flags2 = payload[22];
	nav_pvt->satellite = payload[23];
	nav_pvt->longitude_100ndeg = buf_to_u32_little_endian(payload);
	nav_pvt->latitude_100ndeg = buf_to_u32_little_endian(payload+28);
	nav_pvt->alti_ellipsoid_mm = buf_to_u32_little_endian(payload+32);
	nav_pvt->alti_msl_mm = buf_to_u32_little_endian(payload+36);
	nav_pvt->horizontal_accuracy_mm = buf_to_u32_little_endian(payload+40);
	nav_pvt->vertical_accuracy_mm = buf_to_u32_little_endian(payload+44);
	nav_pvt->northward_velocity_mmps = buf_to_u32_little_endian(payload+48);
	nav_pvt->eastward_velocity_mmps = buf_to_u32_little_endian(payload+52);
	nav_pvt->down_velocity_mmps = buf_to_u32_little_endian(payload+56);
	nav_pvt->groundspeed_mmps = buf_to_u32_little_endian(payload+60);
	nav_pvt->heading_of_motion_10udeg = buf_to_u32_little_endian(payload+64);
	nav_pvt->velocity_accuracy_mmps = buf_to_u32_little_endian(payload+68);
	nav_pvt->heading_accuracy_10udeg = buf_to_u32_little_endian(payload+72);
	nav_pvt->position_dop_centi = buf_to_u16_little_endian(payload+76);
	nav_pvt->flags3 = buf_to_u16_little_endian(payload+78);
	nav_pvt->reserved1 = payload[80];
	nav_pvt->reserved2 = payload[81];
	nav_pvt->reserved3 = payload[82];
	nav_pvt->reserved4 = payload[83];
	nav_pvt->heading_of_vehicle_10udeg = buf_to_u32_little_endian(payload+84);
	nav_pvt->magnetic_declination_cdeg = buf_to_u16_little_endian(payload+88);
	nav_pvt->magnetic_declination_accuracy_cdeg = buf_to_u16_little_endian(payload+90);

	gps->frame_queue_head = (gps->frame_queue_head + 1) % UBX_LEN_FRAME_QUEUE;
	gps->frame_queue_cnt -= 1;

	return true;
}

// TODO : gps arrary로 만들기
//call inside void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
void ubx_uart_rx_cplt_callback(UART_HandleTypeDef *huart, struct ubx_gps *gps)
{
	if (gps->huart == huart) {
		gps->buf_ready = true;
		return ;
	}
}



