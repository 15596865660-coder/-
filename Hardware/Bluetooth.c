#include "Bluetooth.h"
#include "DS3231.h"
#include <string.h>

MedPlan_t Plans[3] = {0}; 
char RxBuffer[64]; 
uint8_t RxIndex = 0;

// 全局标志位，中断里只改变这个值，不执行耗时操作
volatile uint8_t Cmd_Update_Flag = 0; 

// 字符转数字
uint8_t CharToNum(char high, char low) {
    if (high >= '0' && high <= '9' && low >= '0' && low <= '9') {
        return (uint8_t)((high - '0') * 10 + (low - '0'));
    }
    return 0xFF;
}

/**
 * @brief  核心解析逻辑（抛弃不稳定的 strchr，改为基于首字符和固定索引的绝对切片）
 */
void Process_Cmd(void) {
    if (RxIndex == 0) return;

    // ---------------- 指令 1 (对时)：格式如 105192125 ----------------
    if (RxBuffer[0] == '1' && RxIndex >= 9) {
        uint8_t m   = CharToNum(RxBuffer[1], RxBuffer[2]); // 月
        uint8_t d   = CharToNum(RxBuffer[3], RxBuffer[4]); // 日
        uint8_t h   = CharToNum(RxBuffer[5], RxBuffer[6]); // 时
        uint8_t min = CharToNum(RxBuffer[7], RxBuffer[8]); // 分

        if (m > 0 && m <= 12 && h < 24 && min < 60) {
            uint8_t t[7] = {0, min, h, 1, d, m, 26}; 
            DS3231_SetTime(t);
            Cmd_Update_Flag = 1; // 打上标记：对时成功
        }
        RxIndex = 0; 
        memset(RxBuffer, 0, sizeof(RxBuffer)); // 处理完立刻清空
        return;
    }
    
    // ---------------- 指令 2 (闹钟)：格式如 222130 ----------------
    if (RxBuffer[0] == '2' && RxIndex >= 6) {
        // 利用绝对数组下标拿数据，彻底避开连续相同数字（如 22...）造成的指针混乱
        uint8_t id  = RxBuffer[1] - '0';                   // 药盒 ID (0, 1, 2)
        uint8_t h   = CharToNum(RxBuffer[2], RxBuffer[3]); // 时
        uint8_t min = CharToNum(RxBuffer[4], RxBuffer[5]); // 分

        if (id <= 2 && h < 24 && min < 60) {
            Plans[id].Active = 1;
            Plans[id].Hour = h;
            Plans[id].Minute = min;
            Cmd_Update_Flag = 2; // 打上标记：闹钟设置成功
        }
        RxIndex = 0; 
        memset(RxBuffer, 0, sizeof(RxBuffer)); // 处理完立刻清空
        return;
    }

    // 规避异常机制：如果接收到一堆乱七八糟的杂鲁数据导致越界，强制复位
    if (RxIndex > 15) {
        RxIndex = 0;
        memset(RxBuffer, 0, sizeof(RxBuffer));
    }
}

/**
 * @brief  串口中断服务函数
 */
void USART3_IRQHandler(void) {
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {
        uint16_t status = USART3->SR;
        char data = USART_ReceiveData(USART3);
        
        // 发生溢出错误，必须清除，否则中断会卡死
        if (status & USART_FLAG_ORE) {
            (void)USART3->DR; 
        }

        if (RxIndex < 60) {
            RxBuffer[RxIndex++] = data;
            RxBuffer[RxIndex] = '\0';
        }

        // 只要有新字符进入，直接丢给状态判定，由首字母规则做严格的分流过滤
        Process_Cmd();

        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}

// 蓝牙初始化 (保持完全一致)
void Bluetooth_Init(uint32_t baud) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitTypeDef GIS;
    GIS.GPIO_Pin = GPIO_Pin_10; 
    GIS.GPIO_Mode = GPIO_Mode_AF_PP; 
    GIS.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GIS);
    
    GIS.GPIO_Pin = GPIO_Pin_11; 
    GIS.GPIO_Mode = GPIO_Mode_IPU; 
    GPIO_Init(GPIOB, &GIS);
    
    USART_InitTypeDef UIS;
    UIS.USART_BaudRate = baud; 
    UIS.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    UIS.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    UIS.USART_Parity = USART_Parity_No; 
    UIS.USART_StopBits = USART_StopBits_1;
    UIS.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART3, &UIS);
    
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    NVIC_InitTypeDef NIS;
    NIS.NVIC_IRQChannel = USART3_IRQn; 
    NIS.NVIC_IRQChannelPreemptionPriority = 0;
    NIS.NVIC_IRQChannelSubPriority = 0; 
    NIS.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NIS);
    
    USART_Cmd(USART3, ENABLE);
}

// 蓝牙发送字符串 (保持完全一致)
void Bluetooth_SendString(char *String) {
    while (*String) {
        USART_SendData(USART3, *String++);
        while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    }
}

