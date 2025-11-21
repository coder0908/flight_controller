/*
 * attitude.c
 *
 *  Created on: Nov 19, 2025
 *      Author: unRR
 */


#include "init/attitude/attitude.h"
#include "pp.h"

struct cascade_pid g_roll_cpid;
struct cascade_pid g_pitch_cpid;



bool attitude_init()
{
	bool tmp;
	tmp = cascade_pid_init(&g_roll_cpid, 6, 0.00005, 0.0005, 0, -30, 30, 0.7, 0.00005, 0.0001, 0, -30, 30);
	if (!tmp) {
		return false;
	}
	tmp = cascade_pid_init(&g_pitch_cpid, 6, 0.00005, 0.0005, 0, -30, 30, 0.7, 0.00005, 0.0001, 0, -30, 30);
	if (!tmp) {
		return false;
	}

	return true;

}
