/*
 * rp3.c
 *
 *  Created on: 2025. 12. 3.
 *      Author: coder0908
 */

#include "pp.h"
#include "app/tranceiver/rp3/rp3.h"

#define RP3_LEN_RXBUF (CRSF_PLDLEN_MAX + CRSF_LEN_SYNC + CRSF_LEN_LEN)
#define RP3_LEN_TXBUF (CRSF_PLDLEN_MAX + CRSF_LEN_SYNC + CRSF_LEN_LEN)


struct crsf_frame_queue rp3_rx_queue;
struct crsf_frame_queue rp3_tx_queue;

static uint8_t rp3_rx_buf[RP3_LEN_RXBUF];
static uint8_t rp3_tx_buf[RP3_LEN_TXBUF];

static bool rp3_is_rx_buf_full = false;


bool rp3_init()
{
	bool tmp;
	HAL_StatusTypeDef status;
	tmp = crsf_init_frame_queue(&rp3_rx_queue);
	if (!tmp) {
		return false;
	}

	status = HAL_UART_Receive_DMA(&huart2, rp3_rx_buf, RP3_LEN_RXBUF);
	if (status != HAL_OK) {
		return false;
	}

	tmp = crsf_init_frame_queue(&rp3_tx_queue);
	if (!tmp) {
		return false;
	}

	return true;
}

bool rp3_push_frame(struct crsf_frame *frame)
{

}

bool rp3_flush_tx()
{

}



void rp3_parse_frames()
{

}

void rp3_uart_receive_rx_cplt_callback(UART_HandleTypeDef *huart)
{
	if (huart->Instance != huart2.Instance) {
		return;
	}

	rp3_is_rx_buf_full = true;
}

