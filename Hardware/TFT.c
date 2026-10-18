#include "TFT.h"
#include "font.h" 

// 仅内部使用的极简延时
void Soft_Delay_ms_TFT(uint32_t ms) {
    for(volatile uint32_t i = 0; i < ms * 8000; i++); 
}

// ⚡ 硬件 SPI 发送：比软件模拟快极多
static void SPI_WriteByte(uint8_t data) {
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, data);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
}

static void TFT_WriteCmd(uint8_t cmd) { TFT_CS(0); TFT_DC(0); SPI_WriteByte(cmd); TFT_CS(1); }
static void TFT_WriteData(uint8_t data) { TFT_CS(0); TFT_DC(1); SPI_WriteByte(data); TFT_CS(1); }
static void TFT_WriteData16(uint16_t data) { TFT_CS(0); TFT_DC(1); SPI_WriteByte(data >> 8); SPI_WriteByte(data & 0xFF); TFT_CS(1); }

void TFT_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_SPI1, ENABLE);

    GPIO_InitTypeDef GIS;
    GIS.GPIO_Pin = TFT_DC_PIN | TFT_CS_PIN;
    GIS.GPIO_Mode = GPIO_Mode_Out_PP;
    GIS.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TFT_PORT, &GIS);
    
    GIS.GPIO_Pin = TFT_RES_PIN; 
    GPIO_Init(TFT_PORT_RES, &GIS); 

    // ⚡ 关键：将 SCK(PA5) 和 SDA(PA7) 的控制权交给硬件 SPI1
    GIS.GPIO_Pin = TFT_SCK_PIN | TFT_MOSI_PIN;
    GIS.GPIO_Mode = GPIO_Mode_AF_PP; 
    GPIO_Init(TFT_PORT, &GIS);

    SPI_InitTypeDef SIS;
    SPI_StructInit(&SIS);
    SIS.SPI_Direction = SPI_Direction_1Line_Tx;
    SIS.SPI_Mode = SPI_Mode_Master;
    SIS.SPI_DataSize = SPI_DataSize_8b;
    SIS.SPI_CPOL = SPI_CPOL_High;
    SIS.SPI_CPHA = SPI_CPHA_2Edge;
    SIS.SPI_NSS = SPI_NSS_Soft;
    SIS.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; // 9MHz，非常快且稳定
    SIS.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(SPI1, &SIS);
    SPI_Cmd(SPI1, ENABLE);

    TFT_CS(1);
    TFT_RES(1); Soft_Delay_ms_TFT(10);
    TFT_RES(0); Soft_Delay_ms_TFT(50);
    TFT_RES(1); Soft_Delay_ms_TFT(120);

    TFT_WriteCmd(0x11); Soft_Delay_ms_TFT(120); 
    TFT_WriteCmd(0x36); TFT_WriteData(0x00); // 保持你现在的方向
    TFT_WriteCmd(0x3A); TFT_WriteData(0x05); 
    TFT_WriteCmd(0x20); // 保持你现在的极性
    TFT_WriteCmd(0x29); 
    Soft_Delay_ms_TFT(20);
}

void TFT_SetWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    TFT_WriteCmd(0x2A); TFT_WriteData16(x1); TFT_WriteData16(x2);
    TFT_WriteCmd(0x2B); TFT_WriteData16(y1); TFT_WriteData16(y2);
    TFT_WriteCmd(0x2C); 
}

void TFT_Clear(uint16_t color) {
    TFT_SetWindow(0, 0, 239, 319);
    TFT_CS(0); TFT_DC(1);
    uint8_t hi = color >> 8, lo = color & 0xFF;
    for (uint32_t i = 0; i < 76800; i++) { SPI_WriteByte(hi); SPI_WriteByte(lo); }
    TFT_CS(1);
}

void TFT_Fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    TFT_SetWindow(x, y, x + w - 1, y + h - 1);
    TFT_CS(0); TFT_DC(1);
    uint8_t hi = color >> 8, lo = color & 0xFF;
    for (uint32_t i = 0; i < w * h; i++) { SPI_WriteByte(hi); SPI_WriteByte(lo); }
    TFT_CS(1);
}

void TFT_ShowChar_Scale(uint16_t x, uint16_t y, char ch, uint16_t fc, uint16_t bc, uint8_t scale) {
    uint8_t temp, i, j; ch = ch - ' '; 
    for (i = 0; i < 16; i++) {
        temp = ascii_8x16[(uint8_t)ch][i];
        for (j = 0; j < 8; j++) {
            if (temp & 0x80) TFT_Fill(x + j * scale, y + i * scale, scale, scale, fc);
            else TFT_Fill(x + j * scale, y + i * scale, scale, scale, bc);
            temp <<= 1;
        }
    }
}

void TFT_ShowString_Scale(uint16_t x, uint16_t y, char *str, uint16_t fc, uint16_t bc, uint8_t scale) {
    while (*str) {
        TFT_ShowChar_Scale(x, y, *str, fc, bc, scale);
        x += 8 * scale; 
        if (x > 240 - 8 * scale) { x = 0; y += 16 * scale; } 
        str++;
    }
}

