#include "mq2.h"
#include "adc.h"
#include "calibration.h" // Needs Calibration_Is_Done()
#include <math.h>       // For INFINITY, NAN, powf

// --- Global Variable Definition ---
volatile float g_mq2_r0_auto = 0.0f; // Stores auto-calibrated R0

/**
 * @brief Initialize MQ-2 specific resources (if any)
 */
void MQ2_Init(void) {
    // Add specific initialization if needed
}

/**
 * @brief Reads MQ-2 Rs value for calibration process
 * @retval Calculated Rs (Ohm), or INFINITY/NAN on error
 */
float MQ2_Read_Rs_For_Calibration(void) {
    uint16_t adc_value = ADC_Get_MQValue(MQ2_ADC_CHANNEL_INDEX);
    return MQ_Get_Rs(adc_value, MQ2_LOAD_RESISTOR); // Use MQ2 specific RL
}

/**
 * @brief Get estimated Smoke PPM from MQ-2 using auto-calibrated R0
 * @retval Estimated Smoke PPM, or 0 if calibration not done/failed
 */
float MQ2_Get_PPM_Smoke_Auto(void) {
    if (!Calibration_Is_Done() || g_mq2_r0_auto <= 0) {
        return 0.0f; // Calibration not finished or R0 invalid
    }

    uint16_t adc_value = ADC_Get_MQValue(MQ2_ADC_CHANNEL_INDEX);
    float rs = MQ_Get_Rs(adc_value, MQ2_LOAD_RESISTOR); // Use MQ2 specific RL

    if (isinf(rs) || isnan(rs)) {
        return 0.0f; // Rs calculation failed
    }

    return MQ_Calc_PPM_Power(rs, g_mq2_r0_auto, MQ2_SMOKE_PARA_A, MQ2_SMOKE_PARA_B);
}


// --- Common MQ Function Implementations ---
// Place these implementations in ONE file (e.g., here, or mq7.c, or mq_common.c)

float MQ_Get_Voltage(uint16_t adc_value) {
    return ((float)adc_value * STM32_VREF / ADC_RESOLUTION);
}

float MQ_Get_Rs(uint16_t adc_value, float rl_value) {
    float v_out;
    if (adc_value == 0) { adc_value = 1; }
    if (adc_value >= ADC_RESOLUTION) { adc_value = (uint16_t)ADC_RESOLUTION - 1;}
    v_out = MQ_Get_Voltage(adc_value);
    if (v_out < 0.001f) { return INFINITY; }
    // Formula: Rs = RL * (VCC / Vout - 1)
    float rs = rl_value * (MQ_VCC / v_out - 1.0f);
    if (rs < 0) { rs = 0;} // Resistance cannot be negative
    return rs;
}

float MQ_Calc_PPM_Power(float rs, float r0, float para_a, float para_b) {
     if (r0 <= 0 || rs < 0) {
         return 0.0f; // Invalid input 无效输入
     }
    float ratio = rs / r0;
    if (ratio <= 0) {
        // Prevent issues with pow() for non-positive ratio, especially if B is negative 防止非正比例对 pow() 函数产生问题，尤其当 B 为负时
         if (para_b >= 0) { // Usually B is negative for MQ sensors 通常对于 MQ 传感器，B 是负数
             return 0.0f; // Or handle as needed (e.g., very high PPM if B<0) 或者根据需要处理（例如，如果 B<0，则返回非常高的 PPM）
         }
         ratio = 0.0001f; // Use a very small positive number instead 代替使用一个非常小的正数
    }
    // Formula: PPM = A * (Rs/R0)^B 公式: PPM = A * (Rs/R0)^B
    return para_a * powf(ratio, para_b);
}