#ifndef __MQ7_H
#define __MQ7_H

#include "mq_common.h"

// --- MQ-7 Specific Parameters ---
#define MQ7_ADC_CHANNEL_INDEX   ADC_CH_MQ7_INDEX // Defined in adc.h

// --- MQ-7 Load Resistor (Ohm) ---
// ** USER MUST VERIFY AND SET THIS VALUE! Check module schematic/measure. **
// ** It might be different from MQ-2's RL (e.g., 10k Ohm is common for MQ-7) **
#define MQ7_LOAD_RESISTOR    (10000.0f) // Example: 10k Ohm

// MQ-7 CO Curve Parameters (PPM = A * (Rs/R0)^B)
// ** USER MUST VERIFY using sensor datasheet! **
// ** WARNING: Accurate CO reading requires heating cycle, these are for basic demo **
#define MQ7_CO_PARA_A       (99.046f) // Example value A for CO
#define MQ7_CO_PARA_B       (-1.518f) // Example value B for CO

// --- Global Variable (Auto-calibrated R0) ---
extern volatile float g_mq7_r0_auto;

// --- Function Prototypes ---
void MQ7_Init(void);
float MQ7_Read_Rs_For_Calibration(void);
float MQ7_Get_PPM_CO_Auto(void);

#endif /* __MQ7_H */