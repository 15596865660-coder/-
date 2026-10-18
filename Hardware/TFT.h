#ifndef __TFT_H
#define __TFT_H

#include "stm32f10x.h"

// ================== 屏幕引脚定义 ==================
// 你的屏幕没有背光控制脚，所以删除了 BLK 的定义
#define TFT_DC_PIN    GPIO_Pin_3  // PA3: 数据/命令控制 (DC)
#define TFT_CS_PIN    GPIO_Pin_4  // PA4: 片选 (CS)
#define TFT_SCK_PIN   GPIO_Pin_5  // PA5: 模拟时钟 (SCK)
#define TFT_MOSI_PIN  GPIO_Pin_7  // PA7: 模拟数据 (SDA/MOSI)
#define TFT_PORT      GPIOA       // 以上 4 个引脚都在 GPIOA

#define TFT_RES_PIN   GPIO_Pin_12 // PB12: 复位 (RST/RES)
#define TFT_PORT_RES  GPIOB       // 复位引脚在 GPIOB

// ================== 快速引脚操作宏 ==================
#define TFT_CS(x)     GPIO_WriteBit(TFT_PORT, TFT_CS_PIN,  (BitAction)(x))
#define TFT_DC(x)     GPIO_WriteBit(TFT_PORT, TFT_DC_PIN,  (BitAction)(x))
#define TFT_RES(x)    GPIO_WriteBit(TFT_PORT_RES, TFT_RES_PIN, (BitAction)(x))

// ================== 常用 RGB565 颜色 ==================
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define YELLOW  0xFFE0
#define GRAY    0x8410
#define CYAN    0x07FF  // 青色

// ================== 底层驱动与渲染函数 ==================
void TFT_Init(void);
void TFT_Clear(uint16_t color);
void TFT_SetWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void TFT_Fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

// ================== 大字体放大显示函数 ==================
void TFT_ShowChar_Scale(uint16_t x, uint16_t y, char ch, uint16_t fc, uint16_t bc, uint8_t scale);
void TFT_ShowString_Scale(uint16_t x, uint16_t y, char *str, uint16_t fc, uint16_t bc, uint8_t scale);

#endif

