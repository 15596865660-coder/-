#include "AT24C256.h"
#include "MyI2C.h"
#include "Delay.h"

void AT24C256_WriteByte(uint16_t Addr, uint8_t Data) {
    MyI2C_Start();
    MyI2C_SendByte(AT24C256_ADDR); MyI2C_ReceiveAck();
    MyI2C_SendByte(Addr >> 8); MyI2C_ReceiveAck();
    MyI2C_SendByte(Addr & 0xFF); MyI2C_ReceiveAck();
    MyI2C_SendByte(Data); MyI2C_ReceiveAck();
    MyI2C_Stop();
    Delay_ms(5); // 存储掉电保存周期
}

uint8_t AT24C256_ReadByte(uint16_t Addr) {
    uint8_t Data;
    MyI2C_Start();
    MyI2C_SendByte(AT24C256_ADDR); MyI2C_ReceiveAck();
    MyI2C_SendByte(Addr >> 8); MyI2C_ReceiveAck();
    MyI2C_SendByte(Addr & 0xFF); MyI2C_ReceiveAck();
    MyI2C_Start();
    MyI2C_SendByte(AT24C256_ADDR | 0x01); MyI2C_ReceiveAck();
    Data = MyI2C_ReceiveByte(); MyI2C_SendAck(1);
    MyI2C_Stop();
    return Data;
}

