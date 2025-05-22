#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

// --- ����ADCͨ��������DMA������������ ---
// ���� Rank 1 = PA1 (MQ7), Rank 2 = PC0 (MQ2)
#define ADC_CH_MQ7_INDEX    0  // PA1 -> ADC1_IN1  -> DMA[0]
#define ADC_CH_MQ2_INDEX    1  // PC0 -> ADC1_IN10 -> DMA[1]
#define ADC_NUM_CHANNELS    2  // �ܹ���ȡ��ͨ����

// ��������
void ADC1_Init_MQ(void); // ��ʼ������MQ��������ADCͨ��
uint16_t ADC_Get_MQValue(uint8_t channel_index); // ��ȡMQ������ADCֵ

#endif /* __ADC_H */