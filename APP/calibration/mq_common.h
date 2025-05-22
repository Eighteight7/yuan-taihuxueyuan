#ifndef __MQ_COMMON_H
#define __MQ_COMMON_H

#include <math.h>
#include <stdint.h>

// --- 基本物理和硬件参数 ---
#define MQ_VCC              (5.0f)   // 传感器模块供电电压 (V)
#define STM32_VREF          (3.3f)   // STM32 ADC 参考电压 (V)
#define ADC_RESOLUTION      (4095.0f)// ADC 分辨率 (12位)

// --- 辅助函数原型 (将在 mq2.c 或 mq7.c 或单独的 mq_common.c 中实现) ---
float MQ_Get_Voltage(uint16_t adc_value);
float MQ_Get_Rs(uint16_t adc_value, float rl_value); // rl_value 作为参数传入
float MQ_Calc_PPM_Power(float rs, float r0, float para_a, float para_b);

// --- 外部延时函数声明 (需要由调用者提供实现) ---
extern void Delay_ms(volatile uint32_t n_ms);
extern void Delay_us(volatile uint32_t n_us); // 微秒延时, 对DHT11很重要

#endif /* __MQ_COMMON_H */