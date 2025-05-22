#include "mq7.h"
#include "adc.h"
#include "calibration.h" // Needs Calibration_Is_Done()
#include <math.h>       // For INFINITY, NAN

// --- Global Variable Definition ---
volatile float g_mq7_r0_auto = 0.0f; // Stores auto-calibrated R0

/**
 * @brief Initialize MQ-7 specific resources (if any)
 */
void MQ7_Init(void) {
    // Add specific initialization if needed (e.g., heater control pin)
    // WARNING: Simple read does not implement optimal heating cycle!
}

/**
 * @brief Reads MQ-7 Rs value for calibration process
 * @retval Calculated Rs (Ohm), or INFINITY/NAN on error
 */
float MQ7_Read_Rs_For_Calibration(void) {
    uint16_t adc_value = ADC_Get_MQValue(MQ7_ADC_CHANNEL_INDEX);
    return MQ_Get_Rs(adc_value, MQ7_LOAD_RESISTOR); // Use MQ7 specific RL
}

/**
 * @brief Get estimated CO PPM from MQ-7 using auto-calibrated R0
 * @retval Estimated CO PPM, or 0 if calibration not done/failed
 * @warning Does not account for required MQ-7 heating cycle! Accuracy limited.
 */
float MQ7_Get_PPM_CO_Auto(void) {
    if (!Calibration_Is_Done() || g_mq7_r0_auto <= 0) {
        return 0.0f; // 校准未完成或 R0 无效
    }

    uint16_t adc_value = ADC_Get_MQValue(MQ7_ADC_CHANNEL_INDEX);
    float rs = MQ_Get_Rs(adc_value, MQ7_LOAD_RESISTOR); // 使用 MQ7 特定的负载电阻 RL

    if (isinf(rs) || isnan(rs)) {
        return 0.0f; // Rs 计算失败
    }

    return MQ_Calc_PPM_Power(rs, g_mq7_r0_auto, MQ7_CO_PARA_A, MQ7_CO_PARA_B);
}

// Note: Common MQ functions (MQ_Get_Voltage, MQ_Get_Rs, MQ_Calc_PPM_Power)
// should only be implemented ONCE. If implemented in mq2.c, remove/comment them here.