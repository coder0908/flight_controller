/*
 * msgbox.h
 *
 *  Created on: 2025. 12. 4.
 *      Author: coder0908
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef void (*msgbox_callback_fp)(uint8_t msg[], uint8_t msg_len);

bool msgbox_create(const char msgbox_name[], uint8_t msgbox_name_len, uint32_t *msgbox_id);
void msgbox_destroy(int msgbox_id);
bool msgbox_publish(int msgbox_id, uint8_t msg[], uint8_t msg_len);
bool msgbox_subscribe(char msgbox_name[], uint8_t msgbox_name_len, msgbox_callback_fp callback);
void msgbox_dispath(int msgbox_id);
