#ifndef __FONT_H
#define __FONT_H

#include <stdint.h>

/* 8x16 ASCII 字库 (95个可打印字符) */
/* 每个字符由 16 个字节组成 (宽8像素 x 高16像素) */
extern const uint8_t ascii_8x16[95][16];

/* 16x16 中文字库 (项目中用到的汉字) */
/* 每个汉字由 32 个字节组成 (宽16像素 x 高16像素) */
extern const uint8_t chinese_16x16[][32];

#endif

