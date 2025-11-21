/*
 * battery.h
 *
 *  Created on: Nov 19, 2025
 *      Author: unRR
 */

#ifndef __BATTERY_H__
#define __BATTERY_H__

#include <stdbool.h>
#include "battery.h"


extern struct battery g_batt_1;
extern                                                                                                                                                                                                                                                                                               struct battery g_batt_2;

bool batt_init();

extern struct battery g_batt;


#endif /* __BATTERY_H__ */
