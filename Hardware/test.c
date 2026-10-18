/*#include "stm32f10x.h"
#include "Delay.h"
#include "DS3231.h"
#include "Bluetooth.h"
#include "TFT.h"
#include "GUI.h"
#include "Alert.h"
#include "LU6288.h"

// 引入外部 Bluetooth.c 中维护的全局变量和标志位
extern uint8_t DS3231_Time[7]; 
extern MedPlan_t Plans[3]; 
extern volatile uint8_t Cmd_Update_Flag; 

// ================= 状态机定义 =================
typedef enum {
    STATE_IDLE = 0,         
    STATE_VOICE_PROMPT,     
    STATE_WAIT_OPEN,        
    STATE_PUNISH_ALARM,     
    STATE_SUCCESS,          
    STATE_FAILED            
} SystemState;

SystemState currentState = STATE_IDLE;
uint8_t target_plan_idx = 255;         
uint8_t cycle_count = 0;               
uint32_t timer_10ms_tick = 0;          
uint32_t wait_timeout_target = 0;      
uint8_t plan_triggered[3] = {0, 0, 0}; 

// ================= 系统初始化 =================
void System_Init(void) {
    DS3231_Init();            
    TFT_Init();               
    TFT_Clear(0x0000);        
    Bluetooth_Init(9600);     
    Alert_Init();             
    LU6288_Init(9600);        
    
    // 【双板通信核心】：把 PA11 初始化为下拉输入，专门接收 2号板 的高电平信号
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // IPD 下拉，平时稳定为 0
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    LU6288_Say("智能药盒启动，等待连接");
}

int main(void) {
    System_Init();
    
    uint8_t last_sec = 60;    
    
    while (1) {
        // ---------------------------------------------------------
        // 0. 异步业务流：处理蓝牙随时发来的对时/闹钟配置
        // ---------------------------------------------------------
        if (Cmd_Update_Flag == 1) {        
            TFT_Clear(0x0000); 
            Bluetooth_SendString("=> Time OK!\r\n");
            Cmd_Update_Flag = 0; 
            last_sec = 60;                 
        } 
        else if (Cmd_Update_Flag == 2) {   
            TFT_Clear(0x0000); 
            Bluetooth_SendString("=> Alarm OK!\r\n");
            Cmd_Update_Flag = 0; 
            last_sec = 60;       
        }

        // ---------------------------------------------------------
        // 1. 基础时间链：每 100ms 非阻塞读取时钟并刷新 UI
        // ---------------------------------------------------------
        if (timer_10ms_tick % 10 == 0) {
            DS3231_ReadTime(); 
            uint8_t current_sec  = DS3231_Time[0];
            uint8_t current_min  = DS3231_Time[1];
            uint8_t current_hour = DS3231_Time[2];
            
            if (current_hour == 0 && current_min == 0 && current_sec == 0) {
                plan_triggered[0] = 0; 
                plan_triggered[1] = 0; 
                plan_triggered[2] = 0;
            }

            if (current_sec != last_sec) {
                last_sec = current_sec;
                if (currentState == STATE_IDLE) {
                    UI_UpdateDisplay(current_hour, current_min, 255); 
                } else {
                    UI_UpdateDisplay(current_hour, current_min, target_plan_idx); 
                }
            }
        }

        // ---------------------------------------------------------
        // 2. 闹钟扫描链：捕获是否到达预设吃药时间点
        // ---------------------------------------------------------
        if (currentState == STATE_IDLE) {
            uint8_t h = DS3231_Time[2];
            uint8_t m = DS3231_Time[1];
            
            for (int i = 0; i < 3; i++) {
                if (Plans[i].Active == 1 && Plans[i].Hour == h && Plans[i].Minute == m && plan_triggered[i] == 0) {
                    target_plan_idx = i;          
                    plan_triggered[i] = 1;        
                    cycle_count = 0;              
                    
                    TFT_Clear(0x0000);            
                    UI_UpdateDisplay(h, m, target_plan_idx); 
                    
                    currentState = STATE_VOICE_PROMPT; 
                    break;
                }
            }
        }

        // ---------------------------------------------------------
        // 3. 核心状态机
        // ---------------------------------------------------------
        switch (currentState) {
            
            case STATE_VOICE_PROMPT:
                LU6288_Say("请打开药盒，现在是吃药时间");
                Bluetooth_SendString("=> Notice: Time to take medicine!\r\n");
                
                timer_10ms_tick = 0;              
                wait_timeout_target = 12000;      // 120秒安全期
                currentState = STATE_WAIT_OPEN;
                break;

            case STATE_WAIT_OPEN:
                // ?【终极修改1】读取 PA11，只要收到 2号板 发来的高电平 (1)，判定开盒成功！
                if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 1) {
                    currentState = STATE_SUCCESS; 
                } 
                else if (timer_10ms_tick >= wait_timeout_target) {
                    currentState = STATE_PUNISH_ALARM; 
                    timer_10ms_tick = 0;          
                    wait_timeout_target = 3000;   // 30秒惩罚期
                    Alert_On();                   
                }
                break;

            case STATE_PUNISH_ALARM:
                // ?【终极修改2】惩罚期间，同样只认 PA11 传来的高电平 (1)
                if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 1) {
                    Alert_Off();                  // 立刻停震
                    currentState = STATE_SUCCESS; // 逃出生天
                }
                else if (timer_10ms_tick >= wait_timeout_target) {
                    Alert_Off();                  
                    cycle_count++;                
                    
                    if (cycle_count >= 5) {
                        currentState = STATE_FAILED; 
                    } else {
                        currentState = STATE_VOICE_PROMPT; 
                    }
                }
                break;

            case STATE_SUCCESS:
                Bluetooth_SendString("=> Success: Medicine taken.\r\n");
                LU6288_Say("按时吃药，非常棒");
                
                Delay_ms(2500); // 留足播报时间
                
                TFT_Clear(0x0000);                
                last_sec = 60;                    
                currentState = STATE_IDLE;        
                break;

            case STATE_FAILED:
                Bluetooth_SendString("=> Warning: Missed medication!\r\n");
                LU6288_Say("警告，您未按时吃药");
                
                Delay_ms(2500); 
                
                TFT_Clear(0x0000); 
                last_sec = 60;     
                currentState = STATE_IDLE;        
                break;
                
            default:
                currentState = STATE_IDLE;
                break;
        }

        // ---------------------------------------------------------
        // 4. 全局延时心跳
        // ---------------------------------------------------------
        Delay_ms(10); 
        timer_10ms_tick++; 
    }
}*/

