#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include "stdio.h" 

#define USART3_REC_LEN	200  	//�����������ֽ��� 200

// ���ڳ�ʼ��
void USART3_Init(uint32_t baud_rate);

// �����ַ���
void USART_SendString(USART_TypeDef* USARTx, const char *str);

// �����ַ�
uint8_t USART_Receive(USART_TypeDef* USARTx);

// �жϽ������ݵĺ���
void USART3_IRQHandler(void);

// ������ջ�����
extern uint8_t USART_Rx_Buffer[256];  // ���ջ�������СΪ 256 �ֽ�

extern u8  USART3_RX_BUF[USART3_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART3_RX_STA;  
//void USART1_Init(u32 bound);
#endif
