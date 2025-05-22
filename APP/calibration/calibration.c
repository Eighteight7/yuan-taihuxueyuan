#include "calibration.h"
#include "mq_common.h" // Needs delay_ms, INFINITY, NAN
#include "mq2.h"       // Needs MQ2_Read_Rs_For_Calibration and g_mq2_r0_auto
#include "mq7.h"       // Needs MQ7_Read_Rs_For_Calibration and g_mq7_r0_auto
#include <stdio.h>     // For optional printf status messages
#include <math.h>      // For isinf, isnan

// --- Global Variable Definition ---
volatile uint8_t g_calibration_done = 0;

/**
 * @brief Executes MQ sensor preheating and R0 auto-calibration.
 * @warning MUST be called in clean air environment after power-up!
 */
void Calibration_Run_MQ(void) {
    uint16_t i;
    float rs_mq2_sum = 0.0f;
    float rs_mq7_sum = 0.0f;
    uint16_t valid_samples_mq2 = 0;
    uint16_t valid_samples_mq7 = 0;
    float rs_val_mq2, rs_val_mq7;

    g_calibration_done = 0; // Reset status at start

    printf("MQ Sensors Pre-heating... Ensure clean air for %lu seconds.\r\n", CALIBRATION_PREHEAT_TIME_MS / 1000);
    delay_ms(CALIBRATION_PREHEAT_TIME_MS);

    printf("Starting MQ R0 Calibration (%d samples)...\r\n", CALIBRATION_SAMPLES);

    for (i = 0; i < CALIBRATION_SAMPLES; i++) {
        // Read MQ2 Rs
        rs_val_mq2 = MQ2_Read_Rs_For_Calibration();
        if (!isinf(rs_val_mq2) && !isnan(rs_val_mq2) && rs_val_mq2 >= 0) {
            rs_mq2_sum += rs_val_mq2;
            valid_samples_mq2++;
        } else {
             rs_val_mq2 = -1.0f; // Indicate invalid reading for print
        }


        // Read MQ7 Rs
        rs_val_mq7 = MQ7_Read_Rs_For_Calibration();
        if (!isinf(rs_val_mq7) && !isnan(rs_val_mq7) && rs_val_mq7 >= 0) {
            rs_mq7_sum += rs_val_mq7;
            valid_samples_mq7++;
        } else {
             rs_val_mq7 = -1.0f; // Indicate invalid reading for print
        }


        printf("Cal Sample %d/%d: MQ2(Rs=%.1f), MQ7(Rs=%.1f)\r\n",
               i + 1, CALIBRATION_SAMPLES, rs_val_mq2, rs_val_mq7);

        delay_ms(CALIBRATION_SAMPLING_INTERVAL_MS);
    }

    // Calculate and store MQ2 R0
    if (valid_samples_mq2 > 0) {
        g_mq2_r0_auto = rs_mq2_sum / valid_samples_mq2;
        printf("MQ2 R0 Calibration Finished. R0 = %.2f Ohm\r\n", g_mq2_r0_auto);
    } else {
        g_mq2_r0_auto = -1.0f; // Mark as invalid
        printf("MQ2 R0 Calibration Failed! No valid samples.\r\n");
    }

    // Calculate and store MQ7 R0
    if (valid_samples_mq7 > 0) {
        g_mq7_r0_auto = rs_mq7_sum / valid_samples_mq7;
        printf("MQ7 R0 Calibration Finished. R0 = %.2f Ohm\r\n", g_mq7_r0_auto);
    } else {
        g_mq7_r0_auto = -1.0f; // Mark as invalid
        printf("MQ7 R0 Calibration Failed! No valid samples.\r\n");
    }

    g_calibration_done = 1; // Mark calibration as done
    printf("MQ Auto Calibration Finished.\r\n");
}

/**
 * @brief Check if MQ sensor auto-calibration is complete.
 * @retval 1 if done, 0 if not done.
 */
uint8_t Calibration_Is_Done(void) {
    return g_calibration_done;
}