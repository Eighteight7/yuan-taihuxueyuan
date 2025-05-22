#ifndef __MQ2_H
#define __MQ2_H

#include "mq_common.h"

// --- MQ-2 Specific Parameters ---
#define MQ2_ADC_CHANNEL_INDEX   ADC_CH_MQ2_INDEX // Defined in adc.h

// --- MQ-2 Load Resistor (Ohm) ---
// ** USER MUST VERIFY AND SET THIS VALUE! Check module schematic/measure. **
#define MQ2_LOAD_RESISTOR    (5000.0f) // Example: 5k Ohm

// MQ-2 Smoke Curve Parameters (PPM = A * (Rs/R0)^B)
// ** USER MUST VERIFY using sensor datasheet! **
#define MQ2_SMOKE_PARA_A    (99.046f) // Example value A for smoke
#define MQ2_SMOKE_PARA_B    (-1.518f) // Example value B for smoke

// --- Global Variable (Auto-calibrated R0) ---
extern volatile float g_mq2_r0_auto;

// --- Function Prototypes ---
void MQ2_Init(void);
float MQ2_Read_Rs_For_Calibration(void);
float MQ2_Get_PPM_Smoke_Auto(void);

// --- Declare common MQ functions (implementation might be in mq2.c or mq7.c) ---
// Ensure they are defined only once across all .c files
float MQ_Get_Voltage(uint16_t adc_value);
float MQ_Get_Rs(uint16_t adc_value, float rl_value);
float MQ_Calc_PPM_Power(float rs, float r0, float para_a, float para_b);


#endif /* __MQ2_H */