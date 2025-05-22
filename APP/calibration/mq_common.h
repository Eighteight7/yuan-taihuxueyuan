#ifndef __MQ_COMMON_H
#define __MQ_COMMON_H

#include <math.h>
#include <stdint.h>

// --- ���������Ӳ������ ---
#define MQ_VCC              (5.0f)   // ������ģ�鹩���ѹ (V)
#define STM32_VREF          (3.3f)   // STM32 ADC �ο���ѹ (V)
#define ADC_RESOLUTION      (4095.0f)// ADC �ֱ��� (12λ)

// --- ��������ԭ�� (���� mq2.c �� mq7.c �򵥶��� mq_common.c ��ʵ��) ---
float MQ_Get_Voltage(uint16_t adc_value);
float MQ_Get_Rs(uint16_t adc_value, float rl_value); // rl_value ��Ϊ��������
float MQ_Calc_PPM_Power(float rs, float r0, float para_a, float para_b);

// --- �ⲿ��ʱ�������� (��Ҫ�ɵ������ṩʵ��) ---
extern void Delay_ms(volatile uint32_t n_ms);
extern void Delay_us(volatile uint32_t n_us); // ΢����ʱ, ��DHT11����Ҫ

#endif /* __MQ_COMMON_H */