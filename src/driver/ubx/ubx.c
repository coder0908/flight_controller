	/*
 * neom8n_ubx.c
 *
 *  Created on: Jul 12, 2025
 *      Author: coder0908
 */


#include "ubx.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

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

static void ubx_checksum(uint8_t *ck_a, uint8_t *ck_b, uint8_t val)
{
	*ck_a = *ck_a + val;
	*ck_b = *ck_b + *ck_a;
}


void ubx_calc_checksum_buf(const uint8_t *buf, uint16_t buf_len, uint8_t *ck_a, uint8_t *ck_b)
{
	assert(buf);
	assert(ck_a);
	assert(ck_b);

	uint8_t tmp_ck_a = 0;
	uint8_t tmp_ck_b = 0;

	for (uint16_t i=2; i < buf_len-2; i++) {
		ubx_checksum(&tmp_ck_a, &tmp_ck_b, buf[i]);
	}

	*ck_a = tmp_ck_a;
	*ck_b = tmp_ck_b;
}

void ubx_calc_checksum_frame(const struct ubx_frame *frame, uint8_t *ck_a, uint8_t *ck_b)
{
	assert(frame);
	assert(ck_a);
	assert(ck_b);

	const uint8_t *tmp_len = (uint8_t*)(&frame->len);

	ubx_checksum(ck_a, ck_b, (uint8_t)frame->class);
	ubx_checksum(ck_a, ck_b, (uint8_t)frame->id);
	ubx_checksum(ck_a, ck_b, tmp_len[0]);
	ubx_checksum(ck_a, ck_b, tmp_len[1]);

	const uint8_t *const payload = frame->payload;

	for (uint16_t i=0; i<frame->len; i++) {
		ubx_checksum(ck_a, ck_b, payload[i]);
	}
}

bool ubx_init_frame_queue(struct ubx_frame_queue *queue)
{
	assert(queue);

	queue->head = 0;
	queue->len = 0;

	return true;
}

bool ubx_push_frame_queue(struct ubx_frame_queue *queue, const struct ubx_frame *frame)
{
	assert(queue);
	assert(queue->len <= UBX_FRAMESLEN_MAX);

	if (queue->len == UBX_FRAMESLEN_MAX) {
		return false;
	}

	struct ubx_frame *const dest = queue->frames + (queue->head + queue->len) % UBX_FRAMESLEN_MAX;
	*dest = *frame;
	queue->len += 1;

	return true;
}

bool ubx_pop_frame_queue(struct ubx_frame_queue *queue, struct ubx_frame *frame)
{
	assert(queue);
	assert(frame);

	if (queue->len == 0) {
		return false;
	}

	struct ubx_frame *src = queue->frames + queue->head;
	*frame = *src;
	queue->len -= 1;
	queue->head = (queue->head + 1) % UBX_FRAMESLEN_MAX;

	return true;
}

bool ubx_parse_frame(struct ubx_frame *frame, const uint8_t *buf, uint16_t buf_len, uint16_t *read_len)
{
	assert(frame);
	assert(buf);

	if (buf_len < UBX_LEN_METADATA) {
		*read_len = buf_len;
		return false;
	}

	if (buf[0] != UBX_SYNC_CHAR_1) {
		*read_len = 1;
		return false;
	}

	if (buf[1] != UBX_SYNC_CHAR_2) {
		*read_len = 2;
		return false;
	}

	frame->class = buf[2];
	frame->id = buf[3];

	uint16_t *len = (uint16_t*)(buf+4);

	if (buf_len < *len+UBX_LEN_METADATA || *len > UBX_PLDLEN_MAX) {
		*read_len = buf_len;
		return false;
	}
	frame->len = *len;

	*read_len = *len + UBX_LEN_METADATA;

	ubx_calc_checksum_buf(buf, *len+UBX_LEN_METADATA, &(frame->ck_a), &(frame->ck_b));
	if (frame->ck_a != buf[*len+6] || frame->ck_b != buf[*len+7]) {
		return false;
	}

	for (uint16_t i=0; i<*len; i++) {
		frame->payload[i] = buf[i+6];
	}

	return true;
}

void ubx_parse_frames(struct ubx_frame_queue *queue, uint8_t *buf, uint16_t buf_len, uint16_t *read_len)
{
	assert(queue);
	assert(buf);
	assert(read_len);

	struct ubx_frame frame;
	uint16_t tmp_read_len;
	uint16_t total_read_len;
	bool is_parse_success;

	for (total_read_len=0; total_read_len<buf_len; /*intentionally do nothing*/) {
		is_parse_success = ubx_parse_frame(&frame, buf + total_read_len, buf_len - total_read_len, &tmp_read_len);

		total_read_len += tmp_read_len;
		if (is_parse_success) {
			if (!ubx_push_frame_queue(queue, &frame)) {
				break;
			}
		}
	}
	*read_len = total_read_len;
}

bool ubx_flush_frame_queue(struct ubx_frame_queue *queue)
{
	assert(queue);

	queue->len = 0;

	return true;
}


bool ubx_parse_nav_posllh(const struct ubx_frame *frame, struct ubx_nav_posllh *nav_posllh)
{
	assert(frame);
	assert(nav_posllh);


	if (frame->class != UBX_CLASS_NAV || frame->id != UBX_ID_POSLLH) {
		return false;
	}

	if (frame->len != UBX_PLDLEN_NAV_POSLLH) {
		return false;
	}

	const uint8_t *payload = frame->payload;

	nav_posllh->tow_ms = buf_to_u32_little_endian(payload);
	nav_posllh->longitude_100ndeg =  buf_to_u32_little_endian(payload+4);
	nav_posllh->latitude_100ndeg =  buf_to_u32_little_endian(payload+8);
	nav_posllh->alti_ellipsoid_mm =  buf_to_u32_little_endian(payload+12);
	nav_posllh->alti_msl_mm = buf_to_u32_little_endian(payload+16);
	nav_posllh->horizontal_accuracy_mm =  buf_to_u32_little_endian(payload+20);
	nav_posllh->vertical_accuracy_mm =  buf_to_u32_little_endian(payload+24);

	return true;
}

bool ubx_parse_nav_pvt(struct ubx_frame *frame, struct ubx_nav_pvt *nav_pvt)
{
	assert(frame);
	assert(nav_pvt);

	if (frame->class != UBX_CLASS_NAV || frame->id != UBX_ID_PVT) {
		return false;
	}

	if (frame->len != UBX_PLDLEN_NAV_PVT) {
		return false;
	}

	const uint8_t *payload = frame->payload;

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
	nav_pvt->longitude_100ndeg = buf_to_u32_little_endian(payload+24);
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

	return true;
}



