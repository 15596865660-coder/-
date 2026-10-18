#ifndef __LU6288_H
#define __LU6288_H

#include "stm32f10x.h"

// ???????? 9600
void LU6288_Init(uint32_t baudrate);
void LU6288_Say(char *text);

#endif
