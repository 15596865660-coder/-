#ifndef __SENSOR_H
#define __SENSOR_H

#include "stm32f10x.h"

// ================= 宏定义药盒编号 =================
// 使用宏定义可以极大增加 main 函数的可读性
#define BOX_MORNING   1   // 早上药盒对应 PA8
#define BOX_NOON      2   // 中午药盒对应 PA11
#define BOX_EVENING   3   // 晚上药盒对应 PA12

// ================= 函数声明 =================
void Sensor_Init(void);                             // 初始化 3 个药盒的引脚
uint8_t Box_GetKeyState(uint8_t box_num);           // 获取药盒微动开关状态

#endif
