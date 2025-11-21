/*
 * attitude.h
 *
 *  Created on: Nov 19, 2025
 *      Author: unRR
 */

#ifndef INIT_ATTITUDE_ATTITUDE_H_
#define INIT_ATTITUDE_ATTITUDE_H_

#include "pid.h"

extern struct cascade_pid g_roll_cpid;
extern struct cascade_pid g_pitch_cpid;

bool attitude_init();

#endif /* INIT_ATTITUDE_ATTITUDE_H_ */
