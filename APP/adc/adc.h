#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

// --- 定义ADC通道及其在DMA缓冲区的索引 ---
// 假设 Rank 1 = PA1 (MQ7), Rank 2 = PC0 (MQ2)
#define ADC_CH_MQ7_INDEX    0  // PA1 -> ADC1_IN1  -> DMA[0]
#define ADC_CH_MQ2_INDEX    1  // PC0 -> ADC1_IN10 -> DMA[1]
#define ADC_NUM_CHANNELS    2  // 总共读取的通道数

// 函数声明
void ADC1_Init_MQ(void); // 初始化用于MQ传感器的ADC通道
uint16_t ADC_Get_MQValue(uint8_t channel_index); // 获取MQ传感器ADC值

#endif /* __ADC_H */