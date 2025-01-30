#include "HEADER.h"

// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================

void FLOW_METER_TASK(void *pvParameters) {
    const TickType_t xFrequency = pdMS_TO_TICKS(100);  // Frequency of task execution
    TickType_t xLastWakeTime = xTaskGetTickCount();  // Last wake time
    unsigned long PREVIOUS_FM1_PULSE_COUNT = 0;  // Previous pulse count for FM1
    unsigned long PREVIOUS_FM2_PULSE_COUNT = 0;  // Previous pulse count for FM2


    while (1) {
        // Delay for the frequency
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        if (xSemaphoreTake(xMutexDispensedWater, (TickType_t)10) == pdTRUE) {
            // Calculate the differences in pulse counts since the last calculation
            unsigned long FM1_pulseDiff = FM1_PULSE_COUNT - PREVIOUS_FM1_PULSE_COUNT;
            PREVIOUS_FM1_PULSE_COUNT = FM1_PULSE_COUNT;
            float dispensedWaterFM1 = (FM1_pulseDiff * 1000.0) / configManager.config.FM1_HZ;

            if (configManager.config.FLOW_METER_2) {
                if (configManager.config.FLOW_METER_2_CAL) {
                    unsigned long FM2_pulseDiff = FM2_PULSE_COUNT - PREVIOUS_FM2_PULSE_COUNT;
                    PREVIOUS_FM2_PULSE_COUNT = FM2_PULSE_COUNT;
                    float dispensedWaterFM2 = (FM2_pulseDiff * 1000.0) / configManager.config.FM2_HZ;
                    DISPENSED_WATER += dispensedWaterFM2;
                } else {
                    unsigned long FM2_pulseDiff = FM2_PULSE_COUNT - PREVIOUS_FM2_PULSE_COUNT;
                    PREVIOUS_FM2_PULSE_COUNT = FM2_PULSE_COUNT;
                    float dispensedWaterFM2 = (FM2_pulseDiff * 1000.0) / configManager.config.FM2_HZ;
                    DISPENSED_WATER += dispensedWaterFM1 - dispensedWaterFM2;
                }
            } else {
                DISPENSED_WATER += dispensedWaterFM1;
            }

            // Release the mutex
            xSemaphoreGive(xMutexDispensedWater);
        }
    }
}