#include "Alert.h"

// PB0 = 蜂鸣器 (低电平触发)
// PB1 = 震动马达 (高电平触发)

void Alert_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GIS;
    GIS.GPIO_Mode = GPIO_Mode_Out_PP; 
    GIS.GPIO_Speed = GPIO_Speed_50MHz;
    GIS.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_Init(GPIOB, &GIS);
    
    // 【核心修正】默认状态
    GPIO_SetBits(GPIOB, GPIO_Pin_0);   // PB0输出高电平 -> 蜂鸣器关闭
    GPIO_ResetBits(GPIOB, GPIO_Pin_1); // PB1输出低电平 -> 马达关闭
}

void Alert_On(void) {
    // 开启报警
    GPIO_ResetBits(GPIOB, GPIO_Pin_0); // PB0拉低 -> 蜂鸣器尖叫
    GPIO_SetBits(GPIOB, GPIO_Pin_1);   // PB1拉高 -> 马达震动
}

void Alert_Off(void) {
    // 关闭报警
    GPIO_SetBits(GPIOB, GPIO_Pin_0);   // PB0拉高 -> 蜂鸣器静音
    GPIO_ResetBits(GPIOB, GPIO_Pin_1); // PB1拉低 -> 马达停止
}

