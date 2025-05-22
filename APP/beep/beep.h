// beep.h
#ifndef __BEEP_H
#define __BEEP_H

#include "stm32f10x.h" // 包含STM32F10x库头文件

// 定义蜂鸣器连接的GPIO端口和引脚
#define BEEP_PORT              GPIOB
#define BEEP_PIN               GPIO_Pin_1
#define BEEP_PORT_RCC          RCC_APB2Periph_GPIOB // GPIOB挂载在APB2总线上

// 控制蜂鸣器的宏定义 (高电平触发)
// 高电平触发：设置引脚为高电平 -> 蜂鸣器响
#define BEEP_ON()              GPIO_SetBits(BEEP_PORT, BEEP_PIN)
// 高电平触发：设置引脚为低电平 -> 蜂鸣器停
#define BEEP_OFF()             GPIO_ResetBits(BEEP_PORT, BEEP_PIN)

// 函数声明
void BEEP_Init(void); // 蜂鸣器GPIO初始化函数

#endif /* __BEEP_H */