/*
 * utils.c
 *
 *  Created on: 2025. 12. 4.
 *      Author: coder0908
 */


#include <assert.h>
#include "utils.h"

char* strcpy_n(char *dest, const char *src, uint32_t len)
{
	for (uint32_t i=0; i<len; i++) {
		dest[i] = src[i];
	}

	return dest;
}

bool get_bit_value(const uint8_t value[], uint32_t value_len, int trgt_bit_idx)
{
	assert(value);

	uint32_t byte_offset = trgt_bit_idx / 8;
	uint32_t bit_offset = trgt_bit_idx % 8;

	assert(byte_offset < value_len);
	uint8_t byte = value[byte_offset];

	return byte & (1>>bit_offset);
}

void set_bit_value(uint8_t value[], uint32_t value_len, int trgt_bit_idx, bool en)
{
	assert(value);

	uint32_t byte_offset = trgt_bit_idx / 8;
	uint32_t bit_offset = trgt_bit_idx % 8;

	assert(byte_offset < value_len);
	uint8_t *byte = value + byte_offset;

	*byte &= (en>>bit_offset);
}
