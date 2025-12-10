/*
 * gps.h
 *
 *  Created on: 2025. 12. 10.
 *      Author: coder0908
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

bool gps_init();
void gps_loop();
void gps_uart_rx_cplt_callback(UART_HandleTypeDef *huart);
