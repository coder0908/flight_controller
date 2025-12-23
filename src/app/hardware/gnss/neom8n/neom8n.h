/*
 * neom8n.h
 *
 *  Created on: 2025. 12. 3.
 *      Author: coder0908
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

bool neom8n_init();
void neom8n_loop();
void neom8n_uart_rx_cplt_callback(UART_HandleTypeDef *huart);
