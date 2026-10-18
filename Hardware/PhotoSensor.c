#include "stm32f10x.h"                  // Device header

/**
  * @brief  槽型光电开关初始化
  * @param  无
  * @retval 无
  */
void PhotoSensor_Init(void)
{
    // 1. 开启GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 2. 配置PA0为上拉输入
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 内部上拉，无遮挡时默认低电平，有遮挡时被内部拉高
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * @brief  获取槽型光电开关的状态 (用于检测药片掉落)
  * @param  无
  * @retval 1: 有遮挡(药片经过)  0: 无遮挡
  */
uint8_t PhotoSensor_GetState(void)
{
    return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
}

