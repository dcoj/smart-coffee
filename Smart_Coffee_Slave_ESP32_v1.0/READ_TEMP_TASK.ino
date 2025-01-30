#include "HEADER.h"

// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================

void READ_TEMP_TASK(void *pvParameters) {
    for (;;) {
        // Check for error state at the beginning of each loop iteration
        if (ENTER_SAFE_MODE) {
            // Permanently suspend this task
            vTaskSuspend(NULL);
        }

        if (xSemaphoreTake(xMutexTemprature1, portMAX_DELAY) == pdTRUE) {
            TEMP_C_1 = maxSensor1.temperature(RNOMINAL, RREF);
            xSemaphoreGive(xMutexTemprature1);
        }
        if (configManager.config.TEMP_SENSOR_2) {
            if (xSemaphoreTake(xMutexTemprature2, portMAX_DELAY) == pdTRUE) {
                TEMP_C_2 = maxSensor2.temperature(RNOMINAL, RREF);
                xSemaphoreGive(xMutexTemprature2);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}