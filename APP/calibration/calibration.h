#ifndef __CALIBRATION_H
#define __CALIBRATION_H

#include <stdint.h>

// --- Calibration Parameters ---
#define CALIBRATION_PREHEAT_TIME_MS       (60 * 1000) // Preheat time (ms), e.g., 60s. Adjust based on datasheet.
#define CALIBRATION_SAMPLES               (50)        // Number of samples for R0 averaging
#define CALIBRATION_SAMPLING_INTERVAL_MS  (200)       // Interval between samples (ms)

// --- Global Status Flag ---
extern volatile uint8_t g_calibration_done; // 0: Not done, 1: Done

// --- Function Prototypes ---
void Calibration_Run_MQ(void); // Run MQ sensor calibration
uint8_t Calibration_Is_Done(void); // Check if calibration is finished

#endif /* __CALIBRATION_H */