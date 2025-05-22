#include "flame_sensor.h"

/**
 * @brief ��ʼ�����洫���������ӵ� GPIO ���� (PC13).
 * @note ���� PC13 Ϊ��������ģʽ (GPIO_Mode_IPU).
 * ������Ϊ���洫����ģ�� D0 ͨ���ڿ���ʱΪ�ߵ�ƽ, ��⵽����ʱ����.
 * ��������ģʽ����ȷ���ڴ�����δ������Ͽ�ʱ����Ϊ�ߵ�ƽ.
 * ע�� PC13 �������Լ�����������.
 */
void FlameSensor_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // 1. ʹ�� GPIOC ʱ��
    RCC_APB2PeriphClockCmd(FLAME_SENSOR_GPIO_RCC, ENABLE);

    // 2. ���� PC13 Ϊ��������
    GPIO_InitStructure.GPIO_Pin = FLAME_SENSOR_GPIO_PIN;
    // ����Ϊ��������ģʽ. ���ȷ��������ģ������������ҿ���Ϊ��, ��������Ҳ��. ��������׳.
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    // ����ģʽ��, Speed ����ͨ������Ҫ, �����⺯��Ҫ������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(FLAME_SENSOR_GPIO_PORT, &GPIO_InitStructure);
}

/**
 * @brief ��ȡ���洫�������������״̬.
 * @retval uint8_t - FLAME_DETECTED (1) �� FLAME_NOT_DETECTED (0).
 * @note ������汻��⵽ʱ D0 ����͵�ƽ (���Ŷ��� 0).
 *
 */
uint8_t FlameSensor_Read_State(void) {
    // ��ȡ PC13 ������״̬
    if (GPIO_ReadInputDataBit(FLAME_SENSOR_GPIO_PORT, FLAME_SENSOR_GPIO_PIN) == Bit_RESET) {
        // ����Ϊ�͵�ƽ (Bit_RESET = 0), ��ʾ��⵽����
        return FLAME_DETECTED;
    } else {
        // ����Ϊ�ߵ�ƽ (Bit_SET = 1), ��ʾδ��⵽����
        return FLAME_NOT_DETECTED;
    }
}
