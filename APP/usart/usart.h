#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include "stdio.h" 

#define USART3_REC_LEN	200  	//定义最大接收字节数 200

// 串口初始化
void USART3_Init(uint32_t baud_rate);

// 发送字符串
void USART_SendString(USART_TypeDef* USARTx, const char *str);

// 接收字符
uint8_t USART_Receive(USART_TypeDef* USARTx);

// 中断接收数据的函数
void USART3_IRQHandler(void);

// 定义接收缓冲区
extern uint8_t USART_Rx_Buffer[256];  // 接收缓冲区大小为 256 字节

extern u8  USART3_RX_BUF[USART3_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART3_RX_STA;  
//void USART1_Init(u32 bound);
#endif
