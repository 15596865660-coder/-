#include "Sensor.h"

/**
  * @brief  初始化 3 个药盒开关引脚为上拉输入
  * 药盒1(早): PA8, 药盒2(中): PA11, 药盒3(晚): PA12
  */
void Sensor_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitTypeDef GIS;
    GIS.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12;
    GIS.GPIO_Mode = GPIO_Mode_IPU;      // 内部上拉，平时不按为高电平(1)
    GIS.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_Init(GPIOA, &GIS);
}

/**
  * @brief  极简版状态获取：抛弃死锁，依靠外层状态机天然消抖
  */
uint8_t Box_GetKeyState(uint8_t box_num) {
    uint8_t pin_state = 1; 
    
    // 1. 读取对应引脚
    if (box_num == BOX_MORNING) {
        pin_state = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8);
    } 
    else if (box_num == BOX_NOON) {
        pin_state = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11);
    } 
    else if (box_num == BOX_EVENING) {
        pin_state = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12);
    } 
    
    // 2. 只要读到 0 (接地按下)，再连续确认一次防止静电杂波，直接放行！
    // 删除了坑人的 Delay_ms(20) 和永久死锁的 key_released 数组
    if (pin_state == 0) 
    {
        // 极速二次确认（不加延时，专门对抗马达震动带来的接触不良）
        if (box_num == BOX_MORNING)      pin_state = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8);
        else if (box_num == BOX_NOON)    pin_state = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11);
        else if (box_num == BOX_EVENING) pin_state = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12);
        
        if (pin_state == 0) {
            return 1; // 成功捕获按下动作！
        }
    }
    
    return 0; 
}

     