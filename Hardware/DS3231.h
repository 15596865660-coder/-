#ifndef __DS3231_H
#define __DS3231_H
#include "stm32f10x.h"

extern uint8_t DS3231_Time[7]; // [0]秒 [1]分 [2]时 [3]星期 [4]日 [5]月 [6]年

void DS3231_Init(void);
void DS3231_ReadTime(void);
void DS3231_SetTime(uint8_t *Time);

#endif

