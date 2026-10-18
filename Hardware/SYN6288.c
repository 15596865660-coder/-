#include "SYN6288.h"

void SYN6288_Init(void) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GIS;
    GIS.GPIO_Pin = GPIO_Pin_2; GIS.GPIO_Mode = GPIO_Mode_AF_PP; GIS.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GIS);
    USART_InitTypeDef UIS;
    UIS.USART_BaudRate = 9600; UIS.USART_Mode = USART_Mode_Tx;
    UIS.USART_Parity = USART_Parity_No; UIS.USART_StopBits = USART_StopBits_1;
    UIS.USART_WordLength = USART_WordLength_8b; USART_Init(USART2, &UIS);
    USART_Cmd(USART2, ENABLE);
}

void SYN6288_Say(char *text) {
    while(*text) {
        USART_SendData(USART2, *text++);
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    }
}

