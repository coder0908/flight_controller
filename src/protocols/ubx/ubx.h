/*
 * neom8n_ubx.h
 *
 *  Created on: Jul 12, 2025
 *      Author: coder0908
 */

#ifndef __UBX_H__
#define __UBX_H__

#include <stdint.h>
#include <stdbool.h>

#define UBX_SYNC_CHAR_1 	(0xb5)
#define UBX_SYNC_CHAR_2		(0x62)

//ubx field size
#define UBX_LEN_SYNC		2
#define UBX_LEN_LEN 		2
#define UBX_LEN_CLASS		1
#define UBX_LEN_ID		1
#define UBX_LEN_CHECKSUM	2
#define UBX_LEN_METADATA	(UBX_LEN_SYNC+UBX_LEN_LEN+UBX_LEN_CLASS+UBX_LEN_ID+UBX_LEN_CHECKSUM)

//ubx payload size
#define UBX_PLDLEN_MAX 		164
#define	UBX_PLDLEN_NAV_POSLLH	28
#define UBX_PLDLEN_NAV_PVT	92

//ubx sttings
#define UBX_FRAMESLEN_MAX	2


enum ubx_class {
	UBX_CLASS_CFG = 0x06,
	UBX_CLASS_NAV = 0x01
};

enum ubx_id {
	//class cfg
	UBX_ID_CFG = 0x09,
	//class nav
	UBX_ID_POSLLH = 0X02,
	UBX_ID_PVT = 0X07
};

struct ubx_frame {
	uint8_t sync_a;
	uint8_t sync_b;
	enum ubx_class class;
	enum ubx_id id;
	uint16_t len;
	uint8_t payload[UBX_PLDLEN_MAX];
	uint8_t ck_a;
	uint8_t ck_b;
};

struct ubx_frame_queue {
	struct ubx_frame frames[UBX_FRAMESLEN_MAX];
	uint8_t head;
	uint8_t len;
};

struct ubx_nav_posllh {
	uint32_t tow_ms;		//time of week
	int32_t longitude_100ndeg;
	int32_t latitude_100ndeg;
	int32_t alti_ellipsoid_mm;	//alti above ellipsoid
	int32_t alti_msl_mm;	//alti above mean sea level
	uint32_t horizontal_accuracy_mm;	//horizontal accuracy estimate
	uint32_t vertical_accuracy_mm;	//vertical accuracy estimate
};

struct ubx_nav_pvt {
	uint32_t tow_ms;
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minuate;
	uint8_t second;
	uint8_t valid;
	uint32_t time_accuracy_ns;
	int32_t fraction_of_second_ns;			// fraction of second, range-1e9 .. 1e9(UTC)
	uint8_t gnss_fix_type;			// 0 : no fix, 1 : dead reckoning only, 2 : 2d fix, 3 : 3d fix, 4 : gndss + dead reckoning combined, 5 : time only fix
	uint8_t flags;				// fix status flags
	uint8_t flags2;				// additional flags
	uint8_t satellite;
	int32_t longitude_100ndeg;
	int32_t latitude_100ndeg;
	int32_t alti_ellipsoid_mm;
	int32_t alti_msl_mm;
	uint32_t horizontal_accuracy_mm;
	uint32_t vertical_accuracy_mm;
	int32_t northward_velocity_mmps;		// NED
	int32_t eastward_velocity_mmps;		// NED
	int32_t down_velocity_mmps;		// NED
	int32_t groundspeed_mmps;		// 2D
	int32_t heading_of_motion_10udeg;	//2D
	uint32_t velocity_accuracy_mmps;
	uint32_t heading_accuracy_10udeg;	// (both motionand vehicle)
	uint16_t position_dop_centi;
	uint16_t flags3;			// additional flags
	uint8_t reserved1;
	uint8_t reserved2;
	uint8_t reserved3;
	uint8_t reserved4;
	int32_t heading_of_vehicle_10udeg;		//Heading of vehicle (2-D), this is only valid when headVehValid is set, otherwise the  output is set to the heading of motion
	int16_t magnetic_declination_cdeg;		//Magnetic declination. Only supported in ADR 4.10 and later
	uint16_t magnetic_declination_accuracy_cdeg;	//Magnetic declination accuracy. Only supported in ADR 4.10 and later.
};


void ubx_calc_checksum_buf(const uint8_t *buf, uint16_t buf_len, uint8_t *ck_a, uint8_t *ck_b);
void ubx_calc_checksum_frame(const struct ubx_frame *frame, uint8_t *ck_a, uint8_t *ck_b);
bool ubx_init_frame_queue(struct ubx_frame_queue *queue);
bool ubx_push_frame_queue(struct ubx_frame_queue *queue, const struct ubx_frame *frame);
bool ubx_pop_frame_queue(struct ubx_frame_queue *queue, struct ubx_frame *frame);
bool ubx_parse_frame(struct ubx_frame *frame, const uint8_t *buf, uint16_t buf_len, uint16_t *read_len);
void ubx_parse_frames(struct ubx_frame_queue *queue, uint8_t *buf, uint16_t buf_len, uint16_t *read_len);
bool ubx_flush_frame_queue(struct ubx_frame_queue *queue);
bool ubx_parse_nav_posllh(const struct ubx_frame *frame, struct ubx_nav_posllh *nav_posllh);
bool ubx_parse_nav_pvt(struct ubx_frame *frame, struct ubx_nav_pvt *nav_pvt);



/*
 *
 * //uart1
const uint8_t UBX_CFG_PRT[28] = {
		0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00,
		0x00, 0x00, 0xD0, 0x08, 0x00, 0x00, 0x00, 0x96,
		0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x8B, 0x54
};

//NAV PVT
const uint8_t UBX_CFG_MSG[16] = {
		0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x01, 0x07,
		0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x18, 0xE1
};

//5hz
const uint8_t UBX_CFG_RATE[14] = {
		0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xC8, 0x00,
		0x01, 0x00, 0x00, 0x00, 0xDD, 0x68
};


const uint8_t UBX_CFG_CFG[21] = {
		0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0x00, 0x00,
		0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x17, 0x31, 0xBF
};
 *
 * */








#endif /* __UBX_H__ */
