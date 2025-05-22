// main.c
#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "dht11.h"
#include "esp8266.h"
#include "stm32f10x.h"
#include "string.h"
#include "beep.h"
#include "adc.h"
#include "mq2.h"
#include "mq7.h"
#include "flame_sensor.h"
#include "calibration.h"
#include <math.h> // For fmaxf

// --- 加权风险评分相关常量 (重要: 用户必须根据实际情况仔细调整!) ---
// 权重 (影响力百分比), 总和接近 1 (或根据需要调整)
#define WEIGHT_TEMP          (0.30f)  // 温度权重 (30%)
#define WEIGHT_HUMI          (-0.05f) // 湿度权重 (-5%, 湿度降低增加风险)
#define WEIGHT_CO            (0.35f)  // CO 权重 (35%) - 提高影响力
#define WEIGHT_QUAL          (0.35f)  // 可燃气体权重 (35%) - 提高影响力

// 偏差缩放因子 (将物理偏差映射到大致 0-1 范围, 表示达到此偏差算显著)
#define TEMP_SCALE_FACTOR    (30.0f)  // 温升 30 度算作 1.0 的偏差贡献
#define HUMI_SCALE_FACTOR    (40.0f)  // 湿度下降 40%RH 算作 1.0 的偏差贡献
#define CO_PPM_SCALE_FACTOR  (200.0f) // CO 上升 200 PPM 算作 1.0 的偏差贡献 (需要根据基准调整!)
#define QUAL_PPM_SCALE_FACTOR (400.0f) // 可燃气体上升 400 PPM 算作 1.0 的偏差贡献 (需要根据基准调整!)

// 报警评分阈值 (加权评分超过此值则报警)
#define ALARM_SCORE_THRESHOLD (700.0f)   // 示例阈值, 需要大量测试调整

// 其他常量
#define STABILIZATION_DELAY_MS   (5000) // 标定后稳定时间 (ms)

// --- 全局变量存储基准值 ---
float g_baseline_temp = -99.0f;
float g_baseline_humi = -99.0f;
float g_baseline_mq2_ppm = -99.0f;
float g_baseline_mq7_ppm = -99.0f;
volatile uint8_t g_baseline_calculated = 0; // 基准值计算完成标志

// --- 用户定义的全局变量 ---
int qual; int CO; int humi; int temp; int firesign; int fire = 0;

// --- 辅助函数声明 ---
void USART3_SendString(char* str);
void Calculate_Initial_Baselines(void); // 修改函数名

// ... (USART3_SendString 函数实现 - 同前) ...
void USART3_SendString(char* str) { /* ... USART3 Send implementation ... */
    while (*str) {
        while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
        USART_SendData(USART3, (uint8_t)(*str));
        str++;
    }
}

int main() {
    // --- 局部变量 ---
    uint8_t dht_temp_raw = 0;
    uint8_t dht_humi_raw = 0;
    float mq2_ppm_raw = 0.0f;
    float mq7_ppm_raw = 0.0f;
    uint8_t flame_state_raw = FLAME_NOT_DETECTED;
    DHT11_StatusTypeDef dht11_status;
    uint8_t alarm_active = 0;
    float current_risk_score = 0.0f; // 当前风险评分

    // --- 初始化 ---
    SysTick_Init(72);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		esp8266_connect_mqtt();
    LED_Init();
    USART3_Init(115200);
    ADC1_Init_MQ();
    MQ2_Init();
    MQ7_Init();
    DHT11_Init();
    FlameSensor_Init();
    BEEP_Init();
    // ... (ESP8266 复位 和 LED 指示 - 同前) ...
    delay_ms(1000);
    USART3_SendString("AT+RST\r\n");
    delay_ms(2000);
    LED2 = 0; LED1 = 0; delay_ms(500); LED2 = 1; LED1 = 1;

    // --- MQ 传感器自动标定 ---
    // ** 关键: 确保此时处于洁净空气 **
    Calibration_Run_MQ();

    // --- 计算初始基准值 ---
    // ** 关键: 确保此时仍然处于稳定、洁净的空气中 **
    Calculate_Initial_Baselines(); // 调用计算基准函数

    // --- 主循环 ---
    while (1) {
        alarm_active = 0; // 重置报警标志
        current_risk_score = 0.0f; // 重置风险评分

        // --- 1. 读取所有传感器数据 ---
        if (Calibration_Is_Done()) {
            mq2_ppm_raw = MQ2_Get_PPM_Smoke_Auto();
            mq7_ppm_raw = MQ7_Get_PPM_CO_Auto();
        } else {
            mq2_ppm_raw = -1.0f;
            mq7_ppm_raw = -1.0f;
        }
        dht11_status = DHT11_Read_Data(&dht_temp_raw, &dht_humi_raw);
        flame_state_raw = FlameSensor_Read_State();

        // --- 2. 更新全局变量  ---
        qual = (int)mq2_ppm_raw;
        CO = (int)mq7_ppm_raw;
        humi = (int)dht_humi_raw;
        temp = (int)dht_temp_raw;
        firesign = (int)flame_state_raw;
				esp8266_publish_mqtt_parameters(qual,CO,humi,temp);
        // --- 3. 计算加权风险评分 (仅当基准计算完毕且读数有效) ---
        if (g_baseline_calculated && Calibration_Is_Done()) {
            float dev_t = 0.0f, dev_h = 0.0f, dev_co = 0.0f, dev_q = 0.0f;

            // 计算缩放后的偏差 (限制在 0 或以上)
            if (dht11_status == DHT11_OK) {
                dev_t = fmaxf(0.0f, ((float)dht_temp_raw - g_baseline_temp)) / TEMP_SCALE_FACTOR;
                dev_h = fmaxf(0.0f, (g_baseline_humi - (float)dht_humi_raw)) / HUMI_SCALE_FACTOR; // 湿度反向
            }
            if (mq7_ppm_raw >= 0 && g_baseline_mq7_ppm > 0) { // 避免除零
                 dev_co = fmaxf(0.0f, (mq7_ppm_raw - g_baseline_mq7_ppm)) / CO_PPM_SCALE_FACTOR;
                // 或者用相对偏差: dev_co = fmaxf(0.0f, (mq7_ppm_raw - g_baseline_mq7_ppm)) / g_baseline_mq7_ppm; 需要调整权重和阈值
            }
             if (mq2_ppm_raw >= 0 && g_baseline_mq2_ppm > 0) { // 避免除零
                dev_q = fmaxf(0.0f, (mq2_ppm_raw - g_baseline_mq2_ppm)) / QUAL_PPM_SCALE_FACTOR;
                 // 或者用相对偏差: dev_q = fmaxf(0.0f, (mq2_ppm_raw - g_baseline_mq2_ppm)) / g_baseline_mq2_ppm;
            }

            // 计算加权总分
            current_risk_score = (WEIGHT_TEMP * dev_t) +
                                 (WEIGHT_HUMI * dev_h) + // 注意湿度权重为负
                                 (WEIGHT_CO * dev_co) +
                                 (WEIGHT_QUAL * dev_q);

            // 如果评分超过阈值, 设置报警标志
            if (current_risk_score > ALARM_SCORE_THRESHOLD) {
                alarm_active = 1;
            }
        }

        // --- 4. 最终报警判断 (直接火焰优先) ---
        if (flame_state_raw == FLAME_DETECTED) {
            alarm_active = 1; // 直接火焰信号覆盖评分判断
        }

        // 更新全局 fire 标志 (基于最终报警状态)
        fire = alarm_active;
				esp8266_publish_fire(flame_state_raw, alarm_active);

        // --- 5. 控制蜂鸣器 ---
        if (alarm_active == 1) {
            BEEP_ON();
        } else {
            BEEP_OFF();
        }

        // --- 6. 控制 LED 指示灯 (示例) ---
        LED3 = (alarm_active == 1) ? 0 : 1;       // LED1: 报警指示灯
				LED4 = (alarm_active == 1) ? 1 : 0;       // LED1: 报警指示灯
        // --- 循环延时 ---
        delay_ms(2000);
    }
    // return 0;
}


/**
 * @brief 计算传感器的初始基准值 (不再计算阈值)
 * @note 应在 Calibration_Run_MQ() 之后, 且传感器稳定后调用一次.
 * @note ** 必须确保此时环境是稳定、洁净的! **
 */
void Calculate_Initial_Baselines(void) { // 函数名修改
    uint8_t dht_temp_base = 0;
    uint8_t dht_humi_base = 0;
    float mq2_ppm_base = -1.0f;
    float mq7_ppm_base = -1.0f;
    DHT11_StatusTypeDef dht_status_base;
    int retry_count = 0;
    const int max_retries = 3;

    g_baseline_calculated = 0;

    if (!Calibration_Is_Done()) {
        return;
    }
    delay_ms(STABILIZATION_DELAY_MS); // 等待稳定

    // 读取基准值
    retry_count = 0;
    do {
        dht_status_base = DHT11_Read_Data(&dht_temp_base, &dht_humi_base);
        if (dht_status_base == DHT11_OK) break;
        retry_count++;
        delay_ms(500);
    } while (retry_count < max_retries);

    mq2_ppm_base = MQ2_Get_PPM_Smoke_Auto();
    mq7_ppm_base = MQ7_Get_PPM_CO_Auto();

    // 存储基准值 (失败则用默认值)
    if (dht_status_base == DHT11_OK) {
        g_baseline_temp = (float)dht_temp_base;
        g_baseline_humi = (float)dht_humi_base;
    } else {
        g_baseline_temp = 25.0f;
        g_baseline_humi = 50.0f;
    }
    g_baseline_mq2_ppm = (mq2_ppm_base >= 0) ? mq2_ppm_base : 50.0f;
    g_baseline_mq7_ppm = (mq7_ppm_base >= 0) ? mq7_ppm_base : 20.0f;

    g_baseline_calculated = 1; // 标记基准计算完成

   
    LED2 = 0; delay_ms(100); LED2 = 1; delay_ms(100);
    LED2 = 0; delay_ms(100); LED2 = 1;
}

