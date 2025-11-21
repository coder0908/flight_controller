/*
 * crsf.h
 *
 *  Created on: Nov 13, 2025
 *      Author: unRR
 */

#ifndef __CRSF_H__
#define __CRSF_H__

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include <math.h>


//crsf field size
#define CRSF_SIZE_SYNC		1
#define CRSF_SIZE_LEN		1
#define CRSF_SIZE_TYPE		1
#define CRSF_SIZE_CRC		1
#define CRSF_SIZE_METADATA 	(CRSF_SIZE_SYNC+CRSF_SIZE_LEN+CRSF_SIZE_TYPE+CRSF_SIZE_CRC)

#define CRSF_VOLTAGES_VALUE_CNT_MAX 29
#define CRSF_TEMPERATURE_VALUE_CNT_MAX 20
//crsf sync character
#define CRSF_SYNC_CHAR 0xc8

//TODO : enum으로 바꾸기. 타입캐스팅 필요.

enum crsf_type {
	CRSF_TYPE_GPS = 0x02,
	CRSF_TYPE_GPS_TIME = 0x03,
	CRSF_TYPE_GPS_EX = 0x06,
	CRSF_TYPE_VARIOMETER = 0x07,
	CRSF_TYPE_BATTERY = 0x08,
	CRSF_TYPE_BAROMETER = 0x09,
	CRSF_TYPE_AIRSPEED = 0x0A,
	CRSF_TYPE_TEMPERATURE = 0x0D,
	CRSF_TYPE_LINK_STATISTIC = 0x14,
	CRSF_TYPE_RC_CHANNELS = 0x16,
	CRSF_TYPE_ATTITUDE = 0x1E,
	CRSF_TYPE_VOLTAGES = 0x0E
};

//crsf payload size
#define CRSF_PLD_SIZE_MAX 		60
#define CRSF_PLD_SIZE_GPS		15
#define CRSF_PLD_SIZE_GPS_TIME		9
#define CRSF_PLD_SIZE_GPS_EX		20
#define CRSF_PLD_SIZE_VARIOMETER	2
#define CRSF_PLD_SIZE_BATTERY		8
#define CRSF_PLD_SIZE_BAROMETER		3
#define CRSF_PLD_SIZE_AIRSPEED		2
#define CRSF_PLD_SIZE_LINK_STATISTIC	10
#define CRSF_PLD_SIZE_RC_CHANNELS	22
#define CRSF_PLD_SIZE_ATTITUDE		6
#define CRSF_PLD_SIZE_TEMP(temperature_count)	((temperature_count) * 2 + 1)
#define CRSF_PLD_SIZE_VOLTAGES(battery_count)	((battery_count) * 2 + 1)

//crsf lib settings
#define CRSF_LEN_FRAME_QUEUE 4
#define CRSF_UART_TIMEOUT_MS 1000
#define CRSF_BUF_SIZE (CRSF_PLD_SIZE_RC_CHANNELS + CRSF_SIZE_METADATA)


#define RADIAN (180/M_PI)

static inline float DEG_TO_RAD(float degree)
{
	return degree / RADIAN;
}

static inline float RAD_TO_DEG(float radian)
{
	return radian * RADIAN;
}

struct crsf_frame {
	enum crsf_type type;
	uint8_t length;
	uint8_t payload[CRSF_BUF_SIZE-CRSF_SIZE_METADATA];
	uint8_t crc;
};

struct crsf {
	UART_HandleTypeDef *huart;

	uint8_t buf[CRSF_BUF_SIZE];
	uint8_t buf_size;
	bool buf_ready;

	struct crsf_frame frame_queue[CRSF_LEN_FRAME_QUEUE];
	uint8_t frame_queue_head;
	uint8_t frame_queue_cnt;
};

struct crsf_gps {
	int32_t latitude_100ndeg;      	 // degree/1e-7
	int32_t longitude_100ndeg;    	  // degree/1e-7
	uint16_t groundspeed_damph; 	  // decameter/hour
	uint16_t heading_cdeg;      	 // cnetidegree
	uint16_t alti_m;      	// meter
	uint8_t satellites;
};

struct crsf_gps_time {
	int16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint16_t millisecond;
};

struct crsf_gps_ex {
	uint8_t fix_type;      				 // Current GPS fix quality
	int16_t northward_velocity_cmps;        		// Northward (north = positive) Speed [cm/sec]
	int16_t eastward_velocity_cmps;        			// Eastward (east = positive) Speed [cm/sec]
	int16_t vertical_velocity_cmps;       			 // Vertical (up = positive) Speed [cm/sec]
	int16_t horizontal_velocity_accuracy_cmps;    		// Horizontal Speed accuracy cm/sec
	int16_t track_accuracy_deg;      			// Heading accuracy in degrees scaled with 1e-1 degrees times 10)
	int16_t alti_ellipsoid_m;  			// Meters Height above GPS Ellipsoid (not MSL)
	int16_t horizontal_accuracy_cm;         		 // horizontal accuracy in cm
	int16_t vertical_accuracy_cm;          		// vertical accuracy in cm
	uint8_t reserved;
	uint8_t horizontal_dop_deci;          		 // Horizontal dilution of precision,Dimensionless in units of .1.
	uint8_t vertical_dop_deci;           		// vertical dilution of precision, Dimensionless in units of .1.
};

struct crsf_variometer {
	int16_t vertical_velocity_cmps;	// cm/s
};

struct crsf_battery {
	int16_t voltage_10uv;        // Voltage µV	//LSB first
	int16_t current_10ua;        // Current 10 µA	//LSB first
	uint32_t capacity_used_mah; 	// Capacity used (mAh)	actually 24bit, ignore highest 8bit
	uint8_t remaining_percent;      // Battery remaining (percent)
};

struct crsf_voltages {
	uint8_t voltage_src_id;
	uint16_t voltages_mv[CRSF_VOLTAGES_VALUE_CNT_MAX];
};

struct crsf_temperature {
	uint8_t temp_src_id;
	int16_t temps_ddeg[CRSF_TEMPERATURE_VALUE_CNT_MAX];	//deci degree

};

//어려움 나중에 구현
//struct crsf_barometer {
//
//};

struct crsf_airspeed {
	uint16_t airspeed_hmph;		//hectometer/h
};


//variable length
//struct crsf_temperature {
//	uint8_t temp_source_id;		// Identifies the source of the temperature data (e.g., 0 = FC including all ESCs, 1 = Ambient, etc.)
//	int16_t temperature[20]; 	// up to 20 temperature values in deci-degree (tenths of a degree) Celsius (e.g., 250 = 25.0°C, -50 = -5.0°C)
//};

// Center (1500µs) = 992
#pragma pack(push, 1)
struct crsf_rc_channels {
    // (11 bits per channel * 16 channels) = 22 bytes
    unsigned chan1 : 11;
    unsigned chan2 : 11;
    unsigned chan3 : 11;
    unsigned chan4 : 11;
    unsigned chan5 : 11;
    unsigned chan6 : 11;
    unsigned chan7 : 11;
    unsigned chan8 : 11;
    unsigned chan9 : 11;
    unsigned chan10 : 11;
    unsigned chan11 : 11;
    unsigned chan12 : 11;
    unsigned chan13 : 11;
    unsigned chan14 : 11;
    unsigned chan15 : 11;
    unsigned chan16 : 11;
};
#pragma pack(pop)

struct crsf_attitude {
	int16_t pitch_angle_100urad;	// 100micro radian	//LSB first
	int16_t roll_angle_100urad;	// 100micro radian	//LSB first
	int16_t yaw_angle_100urad;	// 100micro radian	//LSB first
};

bool crsf_init(struct crsf *m, UART_HandleTypeDef *huart);
bool crsf_begin(struct crsf *m);

bool crsf_is_data_ready(struct crsf *m);
uint8_t crsf_calc_crc8(const uint8_t *buf, uint8_t size);
bool crsf_parse_gps(struct crsf *m, struct crsf_gps *gps);
bool crsf_parse_rc_channels(struct crsf *m, struct crsf_rc_channels *rc_channels);
bool crsf_parse_attitude(struct crsf *m, struct crsf_attitude *attitude);

bool crsf_send_gps(struct crsf *m, int32_t latitude_100ndeg, int32_t longitude_100ndeg,
		uint16_t groundspeed_damph, uint16_t heading_cdeg, uint16_t altitude_m, uint8_t satellites);
bool crsf_send_gps_ex(struct crsf *m, uint8_t fix_type, int16_t northward_velocity_cmps, int16_t eastward_velocity_cmps,
		int16_t vertical_velocity_cmps, int16_t horizontal_velocity_accuracy_cmps, int16_t track_accuracy_deg, int16_t alti_ellipsoid_m,
		int16_t horizontal_accuracy_cm, int16_t vertical_accuracy_cm, uint8_t horizontal_dop_deci, uint8_t vertical_dop_deci);

bool crsf_send_gps_time(struct crsf *m, int16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minuate, uint8_t second, uint8_t millisecond);
bool crsf_send_attitude(struct crsf *m, int16_t pitch_angle_100urad, int16_t roll_angle_100urad, int16_t yaw_angle_100urad);
bool crsf_send_variometer(struct crsf *m, int16_t vertical_velocity_cmps);
bool crsf_send_battery(struct crsf *m, int16_t voltage_10uv, int16_t current_10ua, uint32_t capacity_used_mah, uint8_t remaining_percet);
bool crsf_send_voltages(struct crsf *m, uint8_t voltage_src_id, uint16_t voltages_mv[], uint8_t voltage_len);
bool crsf_send_temperature(struct crsf *m, uint8_t temp_src_id, int16_t temps_ddeg[], uint8_t temp_len);
bool crsf_flush_rx(struct crsf *m);

void crsf_uart_rx_cplt_callback(UART_HandleTypeDef *huart, struct crsf *m);

#endif /* __CRSF_H__ */
