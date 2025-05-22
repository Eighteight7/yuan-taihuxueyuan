// beep.c
#include "beep.h" // 包含头文件

/*******************************************************************************
* 函 数 名        : BEEP_Init
* 函数功能        : 蜂鸣器GPIO初始化
* 输    入        : 无
* 输    出        : 无
* 说    明        : 配置PB1为推挽输出模式，并初始状态设置为低电平(蜂鸣器不响)
*******************************************************************************/
void BEEP_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; // 声明GPIO初始化结构体变量

    // 1. 使能GPIOB端口的时钟
    RCC_APB2PeriphClockCmd(BEEP_PORT_RCC, ENABLE);

    // 2. 配置GPIO参数
    GPIO_InitStructure.GPIO_Pin = BEEP_PIN;          // 选择 BEEP 引脚 (PB1)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 设置为推挽输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 设置IO口速度为50MHz

    // 3. 初始化GPIOB
    GPIO_Init(BEEP_PORT, &GPIO_InitStructure);

    // 4. 设置初始状态为关闭 (低电平)
    BEEP_OFF(); // 或者 GPIO_ResetBits(BEEP_PORT, BEEP_PIN);
}