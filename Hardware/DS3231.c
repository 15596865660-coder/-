#include "DS3231.h"
#include "MyI2C.h"

uint8_t DS3231_Time[7];

static uint8_t BCD_To_DEC(uint8_t bcd) { return (bcd >> 4) * 10 + (bcd & 0x0F); }
static uint8_t DEC_To_BCD(uint8_t dec) { return ((dec / 10) << 4) | (dec % 10); }

void DS3231_Init(void) { MyI2C_Init(); }

void DS3231_ReadTime(void) {
    MyI2C_Start();
    MyI2C_SendByte(0xD0); MyI2C_ReceiveAck();
    MyI2C_SendByte(0x00); MyI2C_ReceiveAck();
    MyI2C_Start();
    MyI2C_SendByte(0xD1); MyI2C_ReceiveAck();
    for (int i = 0; i < 6; i++) {
        DS3231_Time[i] = BCD_To_DEC(MyI2C_ReceiveByte());
        MyI2C_SendAck(0);
    }
    DS3231_Time[6] = BCD_To_DEC(MyI2C_ReceiveByte());
    MyI2C_SendAck(1); MyI2C_Stop();
}

void DS3231_SetTime(uint8_t *Time) {
    MyI2C_Start();
    MyI2C_SendByte(0xD0); MyI2C_ReceiveAck();
    MyI2C_SendByte(0x00); MyI2C_ReceiveAck();
    for (int i = 0; i < 7; i++) {
        MyI2C_SendByte(DEC_To_BCD(Time[i])); MyI2C_ReceiveAck();
    }
    MyI2C_Stop();
}

