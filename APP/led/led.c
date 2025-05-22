#include "led.h" // ȷ���������� LEDx_PORT, LEDx_PIN, LEDx_PORT_RCC �궨���ͷ�ļ�
#include "stm32f10x.h" // ���� STM32 ���ļ�

/*******************************************************************************
* �� �� ��        : LED_Init
* ��������        : LED ��ʼ������ (��ʼ�� LED1, LED2, LED3, LED4)
* ��    ��        : ��
* ��    ��        : ��
* ˵    ��        : LED1, LED2 ����ԭ�ж˿ں����� (����ͷ�ļ�����)
* LED3 ��ʼ���� PC1
* LED4 ��ʼ���� PC3
* ���� LED ��ʼ��Ϊ�������, ��ʼ״̬Ϊ�ߵ�ƽ (����)
*******************************************************************************/
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; // ����ṹ�����

    // 1. ʹ������ LED ���ڶ˿ڵ�ʱ��
    // ʹ�� | (��λ��) �����ϲ�������Ҫ��ʱ��ʹ��λ
    // ע��: RCC_APB2Periph_GPIOC �� LED3/LED4 ��Ҫ, һ��ʹ��
    RCC_APB2PeriphClockCmd(LED1_PORT_RCC | LED2_PORT_RCC | LED3_PORT_RCC, ENABLE);
    // ע��: ��Ϊ LED3_PORT_RCC �� LED4_PORT_RCC ���� RCC_APB2Periph_GPIOC,
    // ����дһ�� LED3_PORT_RCC �͹���. ��� LED1/LED2 Ҳ�� GPIOC, ��ôֻдһ�� RCC_APB2Periph_GPIOC ����.

    // 2. ���� GPIO ���ģʽ������ (������ LED ��һ��)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   // �����������ģʽ
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // ���ô�������

    // 3. ��ʼ�� LED1
    GPIO_InitStructure.GPIO_Pin = LED1_PIN;          // ѡ�� LED1 ����
    GPIO_Init(LED1_PORT, &GPIO_InitStructure);       // ��ʼ�� LED1 �˿�
    GPIO_SetBits(LED1_PORT, LED1_PIN);               // �� LED1 ���ߣ�Ϩ�� LED

    // 4. ��ʼ�� LED2
    GPIO_InitStructure.GPIO_Pin = LED2_PIN;          // ѡ�� LED2 ����
    GPIO_Init(LED2_PORT, &GPIO_InitStructure);       // ��ʼ�� LED2 �˿�
    GPIO_SetBits(LED2_PORT, LED2_PIN);               // �� LED2 ���ߣ�Ϩ�� LED

    // 5. ��ʼ�� LED3 �� LED4 (���� GPIOC, ���Ժϲ���ʼ��)
    // ������Ҫ��ʼ�������� (PC1 �� PC3)
    GPIO_InitStructure.GPIO_Pin = LED3_PIN | LED4_PIN; // ͬʱѡ�� LED3 �� LED4 ����
    GPIO_Init(LED3_PORT, &GPIO_InitStructure);       // ��ʼ�� LED3/LED4 ���ڵĶ˿� (GPIOC)
    // ͬʱ�� LED3 �� LED4 ���ߣ�Ϩ�� LED
    GPIO_SetBits(LED3_PORT, LED3_PIN | LED4_PIN);

    /*
    // �����ֿ���ʼ�� LED3 �� LED4 (Ч����ͬ�������Գ�):
    // ��ʼ�� LED3 (PC1)
    GPIO_InitStructure.GPIO_Pin = LED3_PIN;
    GPIO_Init(LED3_PORT, &GPIO_InitStructure);
    GPIO_SetBits(LED3_PORT, LED3_PIN);

    // ��ʼ�� LED4 (PC3)
    GPIO_InitStructure.GPIO_Pin = LED4_PIN;
    GPIO_Init(LED4_PORT, &GPIO_InitStructure); // LED4_PORT Ҳ�� GPIOC
    GPIO_SetBits(LED4_PORT, LED4_PIN);
    */
}