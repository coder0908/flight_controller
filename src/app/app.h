/*
 * app.h
 *
 *  Created on: May 5, 2025
 *      Author: mia00
 */

#ifndef __APP_H__
#define __APP_H__

#include "pp.h"
#include "app/battery/battery.h"
#include "app/consur/consur.h"
#include "app/imu/imu.h"
#include "app/attitude/attitude.h"



bool setup();
void loop(void);

#endif /* __APP_H__ */
