#ifndef __FLAME_SENSOR_H
#define __FLAME_SENSOR_H

#include "stm32f10x.h"

// --- ���洫�������Ŷ��� (���ӵ� PC13) ---
#define FLAME_SENSOR_GPIO_PORT      GPIOC
#define FLAME_SENSOR_GPIO_PIN       GPIO_Pin_13
#define FLAME_SENSOR_GPIO_RCC       RCC_APB2Periph_GPIOC // GPIOC �� APB2 ������

// --- ������״̬���� ---
#define FLAME_NOT_DETECTED  (0) // δ��⵽���� (��Ӧ D0 ���Ÿߵ�ƽ)
#define FLAME_DETECTED      (1) // ��⵽����   (��Ӧ D0 ���ŵ͵�ƽ)

// --- ����ԭ�� ---

/**
 * @brief ��ʼ�����洫���������ӵ� GPIO ���� (PC13).
 * @param None
 * @retval None
 * @note �� PC13 ����Ϊ��������ģʽ.
 */
void FlameSensor_Init(void);

/**
 * @brief ��ȡ���洫�������������״̬.
 * @param None
 * @retval uint8_t - FLAME_DETECTED (1) �� FLAME_NOT_DETECTED (0).
 * @note ������汻��⵽ʱ D0 ����͵�ƽ.
 */
uint8_t FlameSensor_Read_State(void);

#endif /* __FLAME_SENSOR_H */