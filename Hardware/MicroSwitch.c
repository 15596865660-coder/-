#include "stm32f10x.h"                  // Device header
#include "Delay.h"                      // 引入江协科技的Delay模块用于消抖

/**
  * @brief  微动开关初始化
  * @param  无
  * @retval 无
  */
void MicroSwitch_Init(void)
{
    // 1. 开启GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 2. 配置PA1为上拉输入 (微动开关接COM到GND，NO到PA1)
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 内部上拉，没按下是高电平(1)，按下是低电平(0)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * @brief  获取微动开关当前的实时电平 (常用于检测药盒盖子是否关紧)
  * @param  无
  * @retval 0: 处于按下状态 (盖子关紧)  1: 处于松开状态 (盖子打开)
  */
uint8_t MicroSwitch_GetLevel(void)
{
    return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2);
}

/**
  * @brief  检测微动开关的单次按下动作 (带消抖和松手检测，类似按键)
  * @param  无
  * @retval 1: 检测到一次完整的按下动作  0: 无动作
  */
uint8_t MicroSwitch_GetClick(void)
{
    uint8_t Switch_State = 0;
    
    // 如果读取到低电平，说明被按下了
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0)
    {
        Delay_ms(20); // 软件消抖
        while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0); // 等待松手
        Delay_ms(20); // 松手消抖
        Switch_State = 1; // 记录一次有效触发
    }
    
    return Switch_State;
}

