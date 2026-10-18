#include "GUI.h"
#include "TFT.h"
#include <stdio.h>

// 刷新主界面，显示巨大时间与当前建议
void UI_UpdateDisplay(uint8_t hour, uint8_t min, uint8_t current_plan_idx) {
    char timeStr[10];
    
    // 中部显示超大时间 (放大4倍，极为清晰)
    sprintf(timeStr, "%02d:%02d", hour, min);
    TFT_ShowString_Scale(40, 60, timeStr, WHITE, BLACK, 4); 
    
    // 底部显示服药建议区 (放大2倍)
    TFT_ShowString_Scale(10, 180, "SUGGESTION:", YELLOW, BLACK, 2);
    
    if (current_plan_idx == 0) {
        TFT_ShowString_Scale(10, 220, "Take Morning Box", GREEN, BLACK, 2);
    } else if (current_plan_idx == 1) {
        TFT_ShowString_Scale(10, 220, "Take Noon Box   ", GREEN, BLACK, 2);
    } else if (current_plan_idx == 2) {
        TFT_ShowString_Scale(10, 220, "Take Evening Box", GREEN, BLACK, 2);
    } else {
        TFT_ShowString_Scale(10, 220, "Waiting...      ", GRAY, BLACK, 2);
    }
}

