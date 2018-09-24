#include "CRC.h"

uint16_t CRC16Calculate(const uint8_t *ptr, uint16_t len)
{
	uint16_t out = 0;
	int bits_read = 0, bit_flag;

	/* Sanity check: */
	if (ptr == 0)
		return 0;

	while (len > 0)
	{
		bit_flag = out >> 15;

		/* Get next bit: */
		out <<= 1;
		out |= (*ptr >> bits_read) & 1; // item a) work from the least significant bits

		/* Increment bit counter: */
		bits_read++;
		if (bits_read > 7)
		{
			bits_read = 0;
			ptr++;
			len--;
		}

		/* Cycle check: */
		if (bit_flag)
			out ^= CRC16;

	}

	// item b) "push out" the last 16 bits
	int i;
	for (i = 0; i < 16; ++i) {
		bit_flag = out >> 15;
		out <<= 1;
		if (bit_flag)
			out ^= CRC16;
	}

	// item c) reverse the bits
	uint16_t crc = 0;
	i = 0x8000;
	int j = 0x0001;
	for (; i != 0; i >>= 1, j <<= 1) {
		if (i & out) crc |= j;
	}

	return crc;
}

uint8_t CRC8Calculate(const uint8_t *ptr, uint16_t len)
{
	unsigned char i;
	unsigned char crc = 0x00; /* 计算的初始CRC值 */

	while (len--)
	{
		crc ^= *ptr++;  /* 每次先于需要计算的数据异或，计算完指向下一个数据 */
		for (i = 8; i > 0; --i)   /* 下面这段计算过程与计算一个字节crc一样 */
		{
			if (crc & 0x80)
				crc = (crc << 1) ^ 0x31;
			else
				crc = (crc << 1);
		}
	}

	return (crc);
}

