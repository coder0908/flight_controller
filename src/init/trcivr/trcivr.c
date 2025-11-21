/*
 * trcivr.c
 *
 *  Created on: Nov 9, 2025
 *      Author: unRR
 */


#include "trcivr.h"
#include "pp.h"


struct crsf g_trcivr;

struct crsf_rc_channels g_rc_channels;

bool trcivr_init()
{
	bool ret;
	ret = crsf_init(&g_trcivr, &huart2);
	if (!ret) {
		return false;
	}
	ret = crsf_begin(&g_trcivr);
	if (!ret) {
		return false;
	}

	return ret;


}
