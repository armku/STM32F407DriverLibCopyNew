#ifndef __CRC_H
#define __CRC_H

#include <stdint.h>

#define CRC16 0x8005

uint16_t CRC16Calculate(const uint8_t *ptr, uint16_t len);

uint8_t CRC8Calculate(const uint8_t *ptr, uint16_t len);

#endif
