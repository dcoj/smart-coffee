#include "HEADER.h"

// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================

void READ_PRESSURE_TASK(void *pvParameters) {
    for (;;) {
        // Check for error state at the beginning of each loop iteration
        if (ENTER_SAFE_MODE) {
            // Permanently suspend this task
            vTaskSuspend(NULL);
        }
        // Update the current reading
        
        // Configure ADC resolution if needed
        analogReadResolution(12);
        // If you haven't already calibrated your ADC, you can get the Vref value from the ESP-IDF espefuse utility.

        float voltage = analogRead(PRESSURE_SENSOR_PIN) * (3.3 / 4095.0); // Replace 3.3 with your actual measured Vref

        float adjustedMinV = configManager.config.PT2_MIN_V / VOLTAGE_DIVIDER_RATIO;
        float adjustedMaxV = configManager.config.PT2_MAX_V / VOLTAGE_DIVIDER_RATIO;

        float pressure = ((voltage - adjustedMinV) * configManager.config.PT2_KPA) / (adjustedMaxV - adjustedMinV);
        PRESSURE_KPA_2 = pressure; // Assign the calculated pressure

        // Add the latest reading to the buffer and increment the index
        voltageReadings[readingIndex] = PRESSURE_KPA_2;
        readingIndex = (readingIndex + 1) % 5; // Wrap the index if it exceeds the buffer size

        // Compute the running average
        float sum = 0.0;
        for (int i = 0; i < 5; ++i) {
            sum += voltageReadings[i];
        }
        if (xSemaphoreTake(xMutexPressure, portMAX_DELAY) == pdTRUE) {
            PRESSURE_KPA_2 = sum / 5.0;
            xSemaphoreGive(xMutexPressure);
        }

        // Wait for 20ms before the next reading
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}