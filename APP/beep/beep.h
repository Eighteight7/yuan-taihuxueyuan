// beep.h
#ifndef __BEEP_H
#define __BEEP_H

#include "stm32f10x.h" // ����STM32F10x��ͷ�ļ�

// ������������ӵ�GPIO�˿ں�����
#define BEEP_PORT              GPIOB
#define BEEP_PIN               GPIO_Pin_1
#define BEEP_PORT_RCC          RCC_APB2Periph_GPIOB // GPIOB������APB2������

// ���Ʒ������ĺ궨�� (�ߵ�ƽ����)
// �ߵ�ƽ��������������Ϊ�ߵ�ƽ -> ��������
#define BEEP_ON()              GPIO_SetBits(BEEP_PORT, BEEP_PIN)
// �ߵ�ƽ��������������Ϊ�͵�ƽ -> ������ͣ
#define BEEP_OFF()             GPIO_ResetBits(BEEP_PORT, BEEP_PIN)

// ��������
void BEEP_Init(void); // ������GPIO��ʼ������

#endif /* __BEEP_H */