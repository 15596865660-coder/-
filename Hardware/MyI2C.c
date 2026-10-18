#include "MyI2C.h"

#define SCL_W(x) GPIO_WriteBit(GPIOB, GPIO_Pin_6, (BitAction)(x))
#define SDA_W(x) GPIO_WriteBit(GPIOB, GPIO_Pin_7, (BitAction)(x))
#define SDA_R()  GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)

void MyI2C_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GIS;
    GIS.GPIO_Mode = GPIO_Mode_Out_OD; 
    GIS.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GIS.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GIS);
    GPIO_SetBits(GPIOB, GPIO_Pin_6 | GPIO_Pin_7);
}

void MyI2C_Start(void) { SDA_W(1); SCL_W(1); SDA_W(0); SCL_W(0); }
void MyI2C_Stop(void) { SDA_W(0); SCL_W(1); SDA_W(1); }

void MyI2C_SendByte(uint8_t Byte) {
    for (uint8_t i = 0; i < 8; i++) {
        SDA_W(Byte & (0x80 >> i));
        SCL_W(1); SCL_W(0);
    }
}

uint8_t MyI2C_ReceiveByte(void) {
    uint8_t Byte = 0x00; SDA_W(1);
    for (uint8_t i = 0; i < 8; i++) {
        SCL_W(1); if (SDA_R()) Byte |= (0x80 >> i); SCL_W(0);
    }
    return Byte;
}

void MyI2C_SendAck(uint8_t AckBit) { SDA_W(AckBit); SCL_W(1); SCL_W(0); }
uint8_t MyI2C_ReceiveAck(void) {
    uint8_t Ack; SDA_W(1); SCL_W(1);
    Ack = SDA_R(); SCL_W(0); return Ack;
}

