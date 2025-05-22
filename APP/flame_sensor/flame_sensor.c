#include "flame_sensor.h"

/**
 * @brief 初始化火焰传感器所连接的 GPIO 引脚 (PC13).
 * @note 配置 PC13 为上拉输入模式 (GPIO_Mode_IPU).
 * 这是因为火焰传感器模块 D0 通常在空闲时为高电平, 检测到火焰时拉低.
 * 上拉输入模式可以确保在传感器未驱动或断开时引脚为高电平.
 * 注意 PC13 的特殊性及其限流能力.
 */
void FlameSensor_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // 1. 使能 GPIOC 时钟
    RCC_APB2PeriphClockCmd(FLAME_SENSOR_GPIO_RCC, ENABLE);

    // 2. 配置 PC13 为上拉输入
    GPIO_InitStructure.GPIO_Pin = FLAME_SENSOR_GPIO_PIN;
    // 设置为上拉输入模式. 如果确定传感器模块输出是推挽且空闲为高, 浮空输入也可. 上拉更健壮.
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    // 输入模式下, Speed 参数通常不重要, 但按库函数要求配置
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(FLAME_SENSOR_GPIO_PORT, &GPIO_InitStructure);
}

/**
 * @brief 读取火焰传感器的数字输出状态.
 * @retval uint8_t - FLAME_DETECTED (1) 或 FLAME_NOT_DETECTED (0).
 * @note 假设火焰被检测到时 D0 输出低电平 (引脚读到 0).
 *
 */
uint8_t FlameSensor_Read_State(void) {
    // 读取 PC13 的输入状态
    if (GPIO_ReadInputDataBit(FLAME_SENSOR_GPIO_PORT, FLAME_SENSOR_GPIO_PIN) == Bit_RESET) {
        // 引脚为低电平 (Bit_RESET = 0), 表示检测到火焰
        return FLAME_DETECTED;
    } else {
        // 引脚为高电平 (Bit_SET = 1), 表示未检测到火焰
        return FLAME_NOT_DETECTED;
    }
}
