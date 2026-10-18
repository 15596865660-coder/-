#ifndef __AT24C256_H
#define __AT24C256_H
#include "stm32f10x.h"

#define AT24C256_ADDR 0xA0

void AT24C256_WriteByte(uint16_t Addr, uint8_t Data);
uint8_t AT24C256_ReadByte(uint16_t Addr);

#endif

