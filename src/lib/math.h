/*
 * math.h
 *
 *  Created on: 2025. 12. 4.
 *      Author: coder0908
 */

#pragma once

#include <math.h>

#define RADIAN (180.0f/M_PI)

static inline float DEG_TO_RAD(float degree)
{
	return degree / RADIAN;
}

static inline float RAD_TO_DEG(float radian)
{
	return radian * RADIAN;
}

