/*
 * neom8n_ubx.h
 *
 *  Created on: Jul 12, 2025
 *      Author: mia00
 */

#ifndef __NEOM8N_UBX_H__
#define __NEOM8N_UBX_H__

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

#define UBX_SYNC_CHAR_1 	(0xb5)
#define UBX_SYNC_CHAR_2		(0x62)

//ubx field size
#define UBX_SIZE_SYNC		2
#define UBX_SIZE_LEN 		2
#define UBX_SIZE_CLASS		1
#define UBX_SIZE_ID		1
#define UBX_SIZE_CRC		2
#define UBX_SIZE_METADATA	(UBX_SIZE_SYNC+UBX_SIZE_LEN+UBX_SIZE_CLASS+UBX_SIZE_ID+UBX_SIZE_CRC)

//ubx payload size
#define UBX_PLD_SIZE_MAX 		164
#define	UBX_PLD_SIZE_NAV_POSLLH		28
#define UBX_PLD_SIZE_NAV_PVT		92

//ubx sttings
#define UBX_BUF_SIZE		(UBX_PLD_SIZE_NAV_PVT + UBX_SIZE_METADATA)
#define UBX_LEN_FRAME_QUEUE	1


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
	enum ubx_class class;
	enum ubx_id id;
	uint16_t length;
	uint8_t payload[UBX_BUF_SIZE-UBX_SIZE_METADATA];
	uint8_t ck_a;
	uint8_t ck_b;
};

struct ubx_gps {
	UART_HandleTypeDef *huart;

	uint8_t buf[UBX_BUF_SIZE];
	uint16_t buf_size;	//max payload length is uint16_t
	bool buf_ready;

	struct ubx_frame frame_queue[UBX_LEN_FRAME_QUEUE];
	uint8_t frame_queue_head;
	uint8_t frame_queue_cnt;
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






bool ubx_send_data(struct ubx_gps *gps, const uint8_t *buf, uint32_t size);
bool ubx_calc_checksum(const uint8_t *buf, uint16_t size, uint8_t *ck_a, uint8_t *ck_b);
bool ubx_gps_init(struct ubx_gps *gps, UART_HandleTypeDef *huart);
bool ubx_gps_begin(struct ubx_gps *gps);
bool ubx_parse_frame(struct ubx_frame *frame, const uint8_t *buf, uint16_t size);
bool ubx_is_data_ready(struct ubx_gps *gps);
bool ubx_flush_rx(struct ubx_gps *gps);
bool ubx_parse_nav_posllh(struct ubx_gps *gps, struct ubx_nav_posllh *nav_posllh);
bool ubx_parse_nav_pvt(struct ubx_gps *gps, struct ubx_nav_pvt *nav_pvt);
void ubx_uart_rx_cplt_callback(UART_HandleTypeDef *huart, struct ubx_gps *gps);




#endif /* __NEOM8N_UBX_H__ */
