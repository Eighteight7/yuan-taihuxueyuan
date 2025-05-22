// beep.c
#include "beep.h" // ����ͷ�ļ�

/*******************************************************************************
* �� �� ��        : BEEP_Init
* ��������        : ������GPIO��ʼ��
* ��    ��        : ��
* ��    ��        : ��
* ˵    ��        : ����PB1Ϊ�������ģʽ������ʼ״̬����Ϊ�͵�ƽ(����������)
*******************************************************************************/
void BEEP_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; // ����GPIO��ʼ���ṹ�����

    // 1. ʹ��GPIOB�˿ڵ�ʱ��
    RCC_APB2PeriphClockCmd(BEEP_PORT_RCC, ENABLE);

    // 2. ����GPIO����
    GPIO_InitStructure.GPIO_Pin = BEEP_PIN;          // ѡ�� BEEP ���� (PB1)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // ����Ϊ�������ģʽ
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // ����IO���ٶ�Ϊ50MHz

    // 3. ��ʼ��GPIOB
    GPIO_Init(BEEP_PORT, &GPIO_InitStructure);

    // 4. ���ó�ʼ״̬Ϊ�ر� (�͵�ƽ)
    BEEP_OFF(); // ���� GPIO_ResetBits(BEEP_PORT, BEEP_PIN);
}