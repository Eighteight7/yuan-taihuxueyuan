#include "SysTick.h"

static u8  fac_us = 0;            // us��ʱ������
static u16 fac_ms = 0;            // ms��ʱ������
volatile uint32_t g_millis = 0;   // ���������

// ��ʼ���ӳٺ���
// SYSTICK��ʱ�ӹ̶�ΪAHBʱ�ӵ�1/8
// SYSCLK: ϵͳʱ��Ƶ��
void SysTick_Init(u8 SYSCLK)
{
    // ���� SysTick ʱ��ԴΪ AHB / 8
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); 

    fac_us = SYSCLK / 8;                  // ÿ΢��ı�����
    fac_ms = (u16)fac_us * 1000;          // ÿ����ı�����
}

// ��ʱ nus ΢��
void delay_us(u32 nus)
{
    u32 temp;
    SysTick->LOAD = nus * fac_us;                // ʱ�����
    SysTick->VAL = 0x00;                         // ��ռ�����
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;    // ��ʼ����

    do
    {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16))); // �ȴ�ʱ�䵽��

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;   // �رռ�����
    SysTick->VAL = 0x00;                         // ��ռ�����
}

// ��ʱ nms ����
void delay_ms(u16 nms)
{
    u32 temp;
    SysTick->LOAD = (u32)nms * fac_ms;            // ʱ�����
    SysTick->VAL = 0x00;                          // ��ռ�����
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;     // ��ʼ����

    do
    {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16)));  // �ȴ�ʱ�䵽��

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;    // �رռ�����
    SysTick->VAL = 0x00;                          // ��ռ�����
}

// ��ȡ�ѹ�ȥ�ĺ�����
uint32_t millis(void)
{
    return g_millis;  // �����ѹ�ȥ�ĺ�����
}
