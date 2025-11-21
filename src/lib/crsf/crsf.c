/*
 * crsf.c
 *
 *  Created on: Nov 13, 2025
 *      Author: unRR
 */


#include "crsf.h"
#include "vmd.h"
#include <stdio.h>
#include "usart.h"
/*
 * sync_char(1), length(1), type(1), payload(length-2), crc(1)
 *
 * length : includes 'type', 'payload','crc'
 * crc : include 'type', 'payload'
 * */


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



static void u16_to_buf_big_endian(uint8_t *buf, uint16_t value)
{
	buf[0] = value >> 8;
	buf[1] = value & 0xff;
}

static void u24_to_buf_big_endian(uint8_t *buf, uint32_t value)
{
	buf[0] = value >> 16;
	buf[1] = (value >> 8) & 0xff;
	buf[2] = value & 0xff;
}

static void u32_to_buf_big_endian(uint8_t *buf, int32_t value)
{
	buf[0] = value >> 24;
	buf[1] = (value >> 16) & 0xff;
	buf[2] = (value >> 8) & 0xff;
	buf[3] = value & 0xff;
}

// TODO : 파리머터 buf를 전체 프레임으로 바꾸기.
//buf : include type, payload
uint8_t crsf_calc_crc8(const uint8_t *buf, uint8_t size)
{
	uint8_t crc = 0;
	for (uint8_t i=0; i<size; i++) {
		crc ^= buf[i];
		for (int j = 0; j < 8; j++) {
			if (crc & 0x80) {
				crc = (crc << 1) ^ 0xd5;

			} else {
				crc <<= 1;
			}
		}
	}

	return crc;
}

bool crsf_init(struct crsf *m, UART_HandleTypeDef *huart)
{
	VMD_ASSERT_PARAM(m);
	VMD_ASSERT_PARAM(huart);

	m->huart = huart;
	m->buf_size = CRSF_BUF_SIZE;
	m->buf_ready = false;
	m->frame_queue_cnt = 0;
	m->frame_queue_head = 0;

	return true;
}

bool crsf_begin(struct crsf *m)
{
	VMD_ASSERT_PARAM(m);

	HAL_StatusTypeDef status;


	status = HAL_UART_Receive_DMA(m->huart, m->buf, m->buf_size);

	if (status != HAL_OK) {
		return false;
	}

	return true;
}

//0 means fail, non zero means total bytes
static uint8_t crsf_parse_frame(struct crsf_frame *frame, const uint8_t *buf, uint8_t size)
{
	VMD_ASSERT_PARAM(frame);
	VMD_ASSERT_PARAM(buf);

	if (size < 2) {
		return 0;
	}
	if (buf[0] != CRSF_SYNC_CHAR) {
		return 0;
	}


	if (size < buf[1]+2 || buf[1] < 2 || CRSF_PLD_SIZE_MAX+2 < buf[1]) {
		return 0;
	}
	frame->length = buf[1];

	frame->type = buf[2];

	uint8_t crc = crsf_calc_crc8(buf+2, frame->length-1);
	if (buf[frame->length+1] != crc) {
		return 0;
	}
	frame->crc = crc;

	for (int i=0; i<frame->length-2; i++) {
		frame->payload[i] = buf[3+i];
	}

	return frame->length+2;


}

bool crsf_is_data_ready(struct crsf *m)
{
	VMD_ASSERT_PARAM(m);

	if (!m->buf_ready) {
		return false;
	}

	m->buf_ready = false;
	const uint8_t * const buf = m->buf;
	const uint8_t buf_size = m->buf_size;

	struct crsf_frame *frame;
	uint8_t tmp_byte_cnt;
	for (uint8_t byte_cnt=0; byte_cnt<buf_size; /*intentionally do nothing*/) {
		if (m->frame_queue_cnt == CRSF_LEN_FRAME_QUEUE) {
			break;
		}

		frame = m->frame_queue + ((m->frame_queue_head + m->frame_queue_cnt) % CRSF_LEN_FRAME_QUEUE);
		tmp_byte_cnt = crsf_parse_frame(frame, buf+byte_cnt, buf_size-byte_cnt);

		//정상적인 패킷이 잘려서 들어온 경우, 비효율.
		if (!tmp_byte_cnt) {
			byte_cnt++;
			continue;
		}

		byte_cnt += tmp_byte_cnt;
		m->frame_queue_cnt += 1;


	}

	HAL_StatusTypeDef status;
	status = HAL_UART_Receive_DMA(m->huart, m->buf, m->buf_size);
	if (status != HAL_OK) {
		return false;
	}


	if (m->frame_queue_cnt == 0) {
		return false;
	}

	return true;
}

//flush only frame_queue not buf
bool crsf_flush_rx(struct crsf *m)
{
	VMD_ASSERT_PARAM(m);

	m->frame_queue_cnt = 0;



	return true;
}


bool crsf_parse_gps(struct crsf *m, struct crsf_gps *gps)
{
	VMD_ASSERT_PARAM(m);
	VMD_ASSERT_PARAM(gps);

	if (m->frame_queue_cnt == 0) {
		return false;
	}

	const struct crsf_frame *const frame = m->frame_queue + m->frame_queue_head;
	const uint8_t *payload = frame->payload;

	if (frame->type != CRSF_TYPE_GPS) {
		return false;
	}

	// TODO : 길이가 더 길어도 확장 바이트 무시 후, 사용 가능하다고 함.
	if (frame->length != CRSF_PLD_SIZE_GPS + 2) {
		return false;
	}


	gps->latitude_100ndeg = buf_to_u32_little_endian(payload);
	gps->longitude_100ndeg = buf_to_u32_little_endian(payload+4);
	gps->groundspeed_damph = buf_to_u16_little_endian(payload+8);
	gps->heading_cdeg = buf_to_u16_little_endian(payload+10);
	gps->alti_m = buf_to_u16_little_endian(payload+12);
	gps->satellites = payload[14];

	gps->alti_m -= 1000;

	m->frame_queue_head = (m->frame_queue_head + 1) % CRSF_LEN_FRAME_QUEUE;
	m->frame_queue_cnt -= 1;



	return true;
}
//
//bool crsf_parse_gps_time(struct crsf_gps_time *gps_time, const struct crsf_frame *frame)
//{
//	VMD_ASSERT_PARAM(gps_time);
//	VMD_ASSERT_PARAM(frame);
//
//}
//
//bool crsf_parse_gps_ex(struct crsf_gps_ex *gps_ex, const struct crsf_frame *frame)
//{
//	VMD_ASSERT_PARAM(frame);
//
//}
//
//bool crsf_parse_variometer(struct crsf_variometer *vario, const struct crsf_frame *frame)
//{
//	VMD_ASSERT_PARAM(frame);
//
//}
//
//bool crsf_parse_battery(struct crsf_battery *batt, const struct crsf_frame *frame)
//{
//	VMD_ASSERT_PARAM(frame);
//
//}
//
//bool crsf_parse_airspeed(struct crsf_airspeed *airspeed, const struct crsf_frame *frame)
//{
//	VMD_ASSERT_PARAM(frame);
//
//}
//
bool crsf_parse_rc_channels(struct crsf *m, struct crsf_rc_channels *rc_channels)
{
	VMD_ASSERT_PARAM(m);
	VMD_ASSERT_PARAM(rc_channels);




	if (m->frame_queue_cnt == 0) {
		return false;
	}

	const struct crsf_frame *const frame = m->frame_queue + m->frame_queue_head;

	if (frame->type != CRSF_TYPE_RC_CHANNELS) {
		return false;
	}

	// TODO : 길이가 더 길어도 확장 바이트 무시 후, 사용 가능하다고 함.
	if (frame->length != CRSF_PLD_SIZE_RC_CHANNELS + 2) {
		return false;
	}

	const struct crsf_rc_channels *tmp = (struct crsf_rc_channels*)(frame->payload);
	*rc_channels = *tmp;

	m->frame_queue_head = (m->frame_queue_head + 1) % CRSF_LEN_FRAME_QUEUE;
	m->frame_queue_cnt -= 1;

	return true;
}

bool crsf_parse_attitude(struct crsf *m, struct crsf_attitude *attitude)
{
	VMD_ASSERT_PARAM(m);
	VMD_ASSERT_PARAM(attitude);

	if (m->frame_queue_cnt == 0) {
		return false;
	}

	const struct crsf_frame *const frame = m->frame_queue + m->frame_queue_head;
	const uint8_t *payload = frame->payload;

	if (frame->type != CRSF_TYPE_ATTITUDE) {
		return false;
	}

	// TODO : 길이가 더 길어도 확장 바이트 무시 후, 사용 가능하다고 함.
	if (frame->length != CRSF_PLD_SIZE_ATTITUDE + 2) {
		return false;
	}

	attitude->pitch_angle_100urad = buf_to_u16_little_endian(payload);
	attitude->roll_angle_100urad = buf_to_u16_little_endian(payload+2);
	attitude->yaw_angle_100urad = buf_to_u16_little_endian(payload+4);

	m->frame_queue_head = (m->frame_queue_head + 1) % CRSF_LEN_FRAME_QUEUE;
	m->frame_queue_cnt -= 1;




	return true;
}

bool crsf_send_gps(struct crsf *m, int32_t latitude_100ndeg, int32_t longitude_100ndeg,
		uint16_t groundspeed_damph, uint16_t heading_cdeg, uint16_t altitude_m, uint8_t satellites)
{
	uint8_t buf[CRSF_PLD_SIZE_GPS + CRSF_SIZE_METADATA] = {0,};

	buf[0] = CRSF_SYNC_CHAR;
	buf[1] = CRSF_PLD_SIZE_GPS + CRSF_SIZE_TYPE + CRSF_SIZE_CRC;
	buf[2] = (uint8_t)CRSF_TYPE_GPS;

	u32_to_buf_big_endian(buf+3, latitude_100ndeg);
	u32_to_buf_big_endian(buf+7, longitude_100ndeg);
	u16_to_buf_big_endian(buf+11, groundspeed_damph);
	u16_to_buf_big_endian(buf+13, heading_cdeg);
	u16_to_buf_big_endian(buf+15, altitude_m+1000);
	buf[17] = satellites;

	buf[18] = crsf_calc_crc8(buf + CRSF_SIZE_SYNC + CRSF_SIZE_LEN, CRSF_PLD_SIZE_GPS + CRSF_SIZE_TYPE);

	HAL_StatusTypeDef status;
	status = HAL_UART_Transmit(m->huart, buf, CRSF_PLD_SIZE_GPS + CRSF_SIZE_METADATA, CRSF_UART_TIMEOUT_MS);
	if (status != HAL_OK) {
		return false;
	}
	return true;
}

bool crsf_send_gps_ex(struct crsf *m, uint8_t fix_type, int16_t northward_velocity_cmps, int16_t eastward_velocity_cmps,
		int16_t vertical_velocity_cmps, int16_t horizontal_velocity_accuracy_cmps, int16_t track_accuracy_deg, int16_t alti_ellipsoid_m,
		int16_t horizontal_accuracy_cm, int16_t vertical_accuracy_cm, uint8_t horizontal_dop_deci, uint8_t vertical_dop_deci)
{
	VMD_ASSERT_PARAM(m);

	uint8_t buf[CRSF_PLD_SIZE_GPS_EX + CRSF_SIZE_METADATA] = {0,};


	buf[0] = CRSF_SYNC_CHAR;
	buf[1] = CRSF_PLD_SIZE_GPS_EX + CRSF_SIZE_TYPE + CRSF_SIZE_CRC;
	buf[2] = (uint8_t)CRSF_TYPE_GPS_EX;

	buf[3] = fix_type;
	u16_to_buf_big_endian(buf+4, northward_velocity_cmps);
	u16_to_buf_big_endian(buf+6, eastward_velocity_cmps);
	u16_to_buf_big_endian(buf+8, vertical_velocity_cmps);
	u16_to_buf_big_endian(buf+10, horizontal_velocity_accuracy_cmps);
	u16_to_buf_big_endian(buf+12, track_accuracy_deg);
	u16_to_buf_big_endian(buf+14, alti_ellipsoid_m);
	u16_to_buf_big_endian(buf+16, horizontal_accuracy_cm);
	u16_to_buf_big_endian(buf+18, vertical_accuracy_cm);
	buf[20] = 0;
	buf[21] = horizontal_dop_deci;
	buf[22] = vertical_dop_deci;

	buf[23] = crsf_calc_crc8(buf + CRSF_SIZE_SYNC + CRSF_SIZE_LEN, CRSF_PLD_SIZE_GPS_EX + CRSF_SIZE_TYPE);

	HAL_StatusTypeDef status;
	status = HAL_UART_Transmit(m->huart, buf, CRSF_PLD_SIZE_GPS_EX + CRSF_SIZE_METADATA, CRSF_UART_TIMEOUT_MS);
	if (status != HAL_OK) {
		return false;
	}
	return true;
}

bool crsf_send_gps_time(struct crsf *m, int16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minuate, uint8_t second, uint8_t millisecond)
{
	VMD_ASSERT_PARAM(m);

	uint8_t buf[CRSF_PLD_SIZE_GPS_TIME + CRSF_SIZE_METADATA] = {0,};


	buf[0] = CRSF_SYNC_CHAR;
	buf[1] = CRSF_PLD_SIZE_GPS_TIME + CRSF_SIZE_TYPE + CRSF_SIZE_CRC;
	buf[2] = (uint8_t)CRSF_TYPE_GPS_TIME;

	u16_to_buf_big_endian(buf+3, year);
	buf[5] = month;
	buf[6] = day;
	buf[7] = hour;
	buf[8] = minuate;
	buf[9] = second;
	u16_to_buf_big_endian(buf+10, millisecond);

	buf[12] = crsf_calc_crc8(buf + CRSF_SIZE_SYNC + CRSF_SIZE_LEN, CRSF_PLD_SIZE_GPS_TIME + CRSF_SIZE_TYPE);

	HAL_StatusTypeDef status;
	status = HAL_UART_Transmit(m->huart, buf, CRSF_PLD_SIZE_GPS_TIME + CRSF_SIZE_METADATA, CRSF_UART_TIMEOUT_MS);
	if (status != HAL_OK) {
		return false;
	}
	return true;

}

bool crsf_send_attitude(struct crsf *m, int16_t pitch_angle_100urad, int16_t roll_angle_100urad, int16_t yaw_angle_100urad)
{
	VMD_ASSERT_PARAM(m);

	uint8_t buf[CRSF_PLD_SIZE_ATTITUDE + CRSF_SIZE_METADATA] = {0,};


	buf[0] = CRSF_SYNC_CHAR;
	buf[1] = CRSF_PLD_SIZE_ATTITUDE + 2;
	buf[2] = (uint8_t)CRSF_TYPE_ATTITUDE;

	u16_to_buf_big_endian(buf+3, pitch_angle_100urad);
	u16_to_buf_big_endian(buf+5, roll_angle_100urad);
	u16_to_buf_big_endian(buf+7, yaw_angle_100urad);

	buf[9] = crsf_calc_crc8(buf+CRSF_SIZE_SYNC + CRSF_SIZE_LEN, CRSF_PLD_SIZE_ATTITUDE + CRSF_SIZE_TYPE);

	HAL_StatusTypeDef status;
	status = HAL_UART_Transmit(m->huart, buf, CRSF_PLD_SIZE_ATTITUDE + CRSF_SIZE_METADATA, CRSF_UART_TIMEOUT_MS);
	if (status != HAL_OK) {
		return false;
	}
	return true;
}



bool crsf_send_variometer(struct crsf *m, int16_t vertical_velocity_cmps)
{
	VMD_ASSERT_PARAM(m);

	uint8_t buf[CRSF_PLD_SIZE_VARIOMETER + CRSF_SIZE_METADATA] = {0,};

	buf[0] = CRSF_SYNC_CHAR;
	buf[1] = CRSF_PLD_SIZE_VARIOMETER + 2;
	buf[2] = (uint8_t)CRSF_TYPE_VARIOMETER;

	u16_to_buf_big_endian(buf+3, vertical_velocity_cmps);

	buf[5] = crsf_calc_crc8(buf+CRSF_SIZE_SYNC + CRSF_SIZE_LEN, CRSF_PLD_SIZE_VARIOMETER + CRSF_SIZE_TYPE);

	HAL_StatusTypeDef status;
	status = HAL_UART_Transmit(m->huart, buf, CRSF_PLD_SIZE_VARIOMETER + CRSF_SIZE_METADATA, CRSF_UART_TIMEOUT_MS);
	if (status != HAL_OK) {
		return false;
	}
	return true;
}

bool crsf_send_battery(struct crsf *m, int16_t voltage_10uv, int16_t current_10ua, uint32_t capacity_used_mah, uint8_t remaining_percet)
{
	VMD_ASSERT_PARAM(m);

	uint8_t buf[CRSF_PLD_SIZE_BATTERY + CRSF_SIZE_METADATA] = {0,};



	buf[0] = CRSF_SYNC_CHAR;
	buf[1] = CRSF_PLD_SIZE_BATTERY + 2;
	buf[2] = (uint8_t)CRSF_TYPE_BATTERY;

	u16_to_buf_big_endian(buf+3, voltage_10uv);
	u16_to_buf_big_endian(buf+5, current_10ua);
	u24_to_buf_big_endian(buf+7, capacity_used_mah);
	buf[10] = remaining_percet;

	buf[11] = crsf_calc_crc8(buf+CRSF_SIZE_SYNC + CRSF_SIZE_LEN, CRSF_PLD_SIZE_BATTERY+CRSF_SIZE_TYPE);

	HAL_StatusTypeDef status;
	status = HAL_UART_Transmit(m->huart, buf, CRSF_PLD_SIZE_BATTERY + CRSF_SIZE_METADATA, CRSF_UART_TIMEOUT_MS);
	if (status != HAL_OK) {
		return false;
	}
	return true;
}

bool crsf_send_voltages(struct crsf *m, uint8_t voltage_src_id, uint16_t voltages_mv[], uint8_t voltage_len)
{
	VMD_ASSERT_PARAM(m);
	VMD_ASSERT_PARAM(voltages_mv);

	if (voltage_len > 29) {
		return false;
	}

	const uint8_t pld_size = CRSF_PLD_SIZE_VOLTAGES(voltage_len);

	uint8_t buf[CRSF_VOLTAGES_VALUE_CNT_MAX * 2 + 1 + CRSF_SIZE_METADATA] = {0,};

	buf[0] = CRSF_SYNC_CHAR;
	buf[1] = pld_size + 2;
	buf[2] = (uint8_t)CRSF_TYPE_VOLTAGES;

	buf[3] = voltage_src_id;
	uint8_t voltages_idx;
	for (voltages_idx=0; voltages_idx<voltage_len; voltages_idx++) {
		u16_to_buf_big_endian(buf+4+(voltages_idx*2), voltages_mv[voltages_idx]);
	}


	buf[3+pld_size] = crsf_calc_crc8(buf+CRSF_SIZE_SYNC + CRSF_SIZE_LEN, pld_size+CRSF_SIZE_TYPE);

	HAL_StatusTypeDef status;
	status = HAL_UART_Transmit(m->huart, buf, pld_size + CRSF_SIZE_METADATA, CRSF_UART_TIMEOUT_MS);
	if (status != HAL_OK) {
		return false;
	}
	return true;
}

bool crsf_send_temperature(struct crsf *m, uint8_t temp_src_id, int16_t temps_ddeg[], uint8_t temp_len)
{
	VMD_ASSERT_PARAM(m);
	VMD_ASSERT_PARAM(temps_ddeg);

	if (temp_len > 20) {
		return false;
	}

	const uint8_t pld_size = CRSF_PLD_SIZE_TEMP(temp_len);

	uint8_t buf[CRSF_TEMPERATURE_VALUE_CNT_MAX* 2 + 1 + CRSF_SIZE_METADATA] = {0,};

	buf[0] = CRSF_SYNC_CHAR;
	buf[1] = pld_size + 2;
	buf[2] = (uint8_t)CRSF_TYPE_TEMPERATURE;

	buf[3] = temp_src_id;
	uint8_t temps_idx;
	for (temps_idx=0; temps_idx<temp_len; temps_idx++) {
		u16_to_buf_big_endian(buf+4+(temps_idx*2), temps_ddeg[temps_idx]);
	}


	buf[3+pld_size] = crsf_calc_crc8(buf+CRSF_SIZE_SYNC + CRSF_SIZE_LEN, pld_size+CRSF_SIZE_TYPE);

	HAL_StatusTypeDef status;
	status = HAL_UART_Transmit(m->huart, buf, pld_size + CRSF_SIZE_METADATA, CRSF_UART_TIMEOUT_MS);
	if (status != HAL_OK) {
		return false;
	}
	return true;
}

void crsf_uart_rx_cplt_callback(UART_HandleTypeDef *huart, struct crsf *m)
{
	if (huart->Instance == m->huart->Instance) {
		m->buf_ready = true;
		return;
	}

}












