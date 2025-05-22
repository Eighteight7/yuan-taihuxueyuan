#include "SysTick.h"

static u8  fac_us = 0;            // us延时倍乘数
static u16 fac_ms = 0;            // ms延时倍乘数
volatile uint32_t g_millis = 0;   // 毫秒计数器

// 初始化延迟函数
// SYSTICK的时钟固定为AHB时钟的1/8
// SYSCLK: 系统时钟频率
void SysTick_Init(u8 SYSCLK)
{
    // 配置 SysTick 时钟源为 AHB / 8
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); 

    fac_us = SYSCLK / 8;                  // 每微秒的倍乘数
    fac_ms = (u16)fac_us * 1000;          // 每毫秒的倍乘数
}

// 延时 nus 微秒
void delay_us(u32 nus)
{
    u32 temp;
    SysTick->LOAD = nus * fac_us;                // 时间加载
    SysTick->VAL = 0x00;                         // 清空计数器
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;    // 开始倒数

    do
    {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16))); // 等待时间到达

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;   // 关闭计数器
    SysTick->VAL = 0x00;                         // 清空计数器
}

// 延时 nms 毫秒
void delay_ms(u16 nms)
{
    u32 temp;
    SysTick->LOAD = (u32)nms * fac_ms;            // 时间加载
    SysTick->VAL = 0x00;                          // 清空计数器
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;     // 开始倒数

    do
    {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16)));  // 等待时间到达

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;    // 关闭计数器
    SysTick->VAL = 0x00;                          // 清空计数器
}

// 获取已过去的毫秒数
uint32_t millis(void)
{
    return g_millis;  // 返回已过去的毫秒数
}
