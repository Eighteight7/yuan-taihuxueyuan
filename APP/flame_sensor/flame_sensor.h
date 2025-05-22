#ifndef __FLAME_SENSOR_H
#define __FLAME_SENSOR_H

#include "stm32f10x.h"

// --- 火焰传感器引脚定义 (连接到 PC13) ---
#define FLAME_SENSOR_GPIO_PORT      GPIOC
#define FLAME_SENSOR_GPIO_PIN       GPIO_Pin_13
#define FLAME_SENSOR_GPIO_RCC       RCC_APB2Periph_GPIOC // GPIOC 在 APB2 总线上

// --- 传感器状态定义 ---
#define FLAME_NOT_DETECTED  (0) // 未检测到火焰 (对应 D0 引脚高电平)
#define FLAME_DETECTED      (1) // 检测到火焰   (对应 D0 引脚低电平)

// --- 函数原型 ---

/**
 * @brief 初始化火焰传感器所连接的 GPIO 引脚 (PC13).
 * @param None
 * @retval None
 * @note 将 PC13 配置为上拉输入模式.
 */
void FlameSensor_Init(void);

/**
 * @brief 读取火焰传感器的数字输出状态.
 * @param None
 * @retval uint8_t - FLAME_DETECTED (1) 或 FLAME_NOT_DETECTED (0).
 * @note 假设火焰被检测到时 D0 输出低电平.
 */
uint8_t FlameSensor_Read_State(void);

#endif /* __FLAME_SENSOR_H */