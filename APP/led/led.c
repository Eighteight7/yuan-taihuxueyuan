#include "led.h" // 确保包含了有 LEDx_PORT, LEDx_PIN, LEDx_PORT_RCC 宏定义的头文件
#include "stm32f10x.h" // 包含 STM32 库文件

/*******************************************************************************
* 函 数 名        : LED_Init
* 函数功能        : LED 初始化函数 (初始化 LED1, LED2, LED3, LED4)
* 输    入        : 无
* 输    出        : 无
* 说    明        : LED1, LED2 保持原有端口和引脚 (需在头文件定义)
* LED3 初始化在 PC1
* LED4 初始化在 PC3
* 所有 LED 初始化为推挽输出, 初始状态为高电平 (灯灭)
*******************************************************************************/
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; // 定义结构体变量

    // 1. 使能所有 LED 所在端口的时钟
    // 使用 | (按位或) 操作合并所有需要的时钟使能位
    // 注意: RCC_APB2Periph_GPIOC 被 LED3/LED4 需要, 一起使能
    RCC_APB2PeriphClockCmd(LED1_PORT_RCC | LED2_PORT_RCC | LED3_PORT_RCC, ENABLE);
    // 注意: 因为 LED3_PORT_RCC 和 LED4_PORT_RCC 都是 RCC_APB2Periph_GPIOC,
    // 所以写一个 LED3_PORT_RCC 就够了. 如果 LED1/LED2 也在 GPIOC, 那么只写一个 RCC_APB2Periph_GPIOC 即可.

    // 2. 配置 GPIO 输出模式和速率 (对所有 LED 都一样)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   // 设置推挽输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 设置传输速率

    // 3. 初始化 LED1
    GPIO_InitStructure.GPIO_Pin = LED1_PIN;          // 选择 LED1 引脚
    GPIO_Init(LED1_PORT, &GPIO_InitStructure);       // 初始化 LED1 端口
    GPIO_SetBits(LED1_PORT, LED1_PIN);               // 将 LED1 拉高，熄灭 LED

    // 4. 初始化 LED2
    GPIO_InitStructure.GPIO_Pin = LED2_PIN;          // 选择 LED2 引脚
    GPIO_Init(LED2_PORT, &GPIO_InitStructure);       // 初始化 LED2 端口
    GPIO_SetBits(LED2_PORT, LED2_PIN);               // 将 LED2 拉高，熄灭 LED

    // 5. 初始化 LED3 和 LED4 (都在 GPIOC, 可以合并初始化)
    // 配置需要初始化的引脚 (PC1 和 PC3)
    GPIO_InitStructure.GPIO_Pin = LED3_PIN | LED4_PIN; // 同时选择 LED3 和 LED4 引脚
    GPIO_Init(LED3_PORT, &GPIO_InitStructure);       // 初始化 LED3/LED4 所在的端口 (GPIOC)
    // 同时将 LED3 和 LED4 拉高，熄灭 LED
    GPIO_SetBits(LED3_PORT, LED3_PIN | LED4_PIN);

    /*
    // 如果想分开初始化 LED3 和 LED4 (效果相同，代码稍长):
    // 初始化 LED3 (PC1)
    GPIO_InitStructure.GPIO_Pin = LED3_PIN;
    GPIO_Init(LED3_PORT, &GPIO_InitStructure);
    GPIO_SetBits(LED3_PORT, LED3_PIN);

    // 初始化 LED4 (PC3)
    GPIO_InitStructure.GPIO_Pin = LED4_PIN;
    GPIO_Init(LED4_PORT, &GPIO_InitStructure); // LED4_PORT 也是 GPIOC
    GPIO_SetBits(LED4_PORT, LED4_PIN);
    */
}