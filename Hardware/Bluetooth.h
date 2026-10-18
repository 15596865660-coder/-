#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include "stm32f10x.h"

// 计划结构体：ID(0=早,1=中,2=晚)
typedef struct {
    uint8_t Active;
    uint8_t BoxNum;
    uint8_t Hour;
    uint8_t Minute;
    uint8_t Dose;
    uint8_t IsTaken;
} MedPlan_t;

extern MedPlan_t Plans[3]; // 全局变量，让 main.c 也能看到闹钟计划

void Bluetooth_Init(uint32_t baud);
void Bluetooth_SendString(char *String);

#endif

