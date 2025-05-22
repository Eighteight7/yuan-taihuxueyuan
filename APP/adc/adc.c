#include "adc.h"

volatile uint16_t g_adc_mq_converted_values[ADC_NUM_CHANNELS]; // �洢MQ������DMA���

/**
 * @brief ��ʼ��ADC1��ʹ��DMA��ȡ PA1(MQ7) �� PC0(MQ2)
 */
void ADC1_Init_MQ(void) {
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    // 1. ʹ��ʱ�� (GPIOA, GPIOC for Pins, ADC1, DMA1)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // 2. ����GPIOΪģ������
    // PA1 -> ADC1_IN1 (MQ7)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    // PC0 -> ADC1_IN10 (MQ2)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // 3. ����DMA1ͨ��1 for ADC1
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)g_adc_mq_converted_values;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = ADC_NUM_CHANNELS;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel1, ENABLE);

    // 4. ����ADC1
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = ADC_NUM_CHANNELS;
    ADC_Init(ADC1, &ADC_InitStructure);

    // 5. ����ADC����ͨ���������ʱ�� (��Rank˳��)
    // Rank 1: Channel 1 (PA1 - MQ7)
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_71Cycles5);
    // Rank 2: Channel 10 (PC0 - MQ2)
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 2, ADC_SampleTime_71Cycles5);

    // 6. ʹ��ADC1��DMA����
    ADC_DMACmd(ADC1, ENABLE);

    // 7. ʹ��ADC1
    ADC_Cmd(ADC1, ENABLE);

    // 8. ADCУ׼
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));

    // 9. ����ADCת��
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/**
 * @brief ��ȡָ��MQ������ͨ����ADCֵ
 * @param channel_index: ͨ����DMA�����е����� (��adc.h����)
 * @retval 12λADCֵ (0-4095), ��������Ч����0
 */
uint16_t ADC_Get_MQValue(uint8_t channel_index) {
    if (channel_index < ADC_NUM_CHANNELS) {
        return g_adc_mq_converted_values[channel_index];
    }
    return 0;
}