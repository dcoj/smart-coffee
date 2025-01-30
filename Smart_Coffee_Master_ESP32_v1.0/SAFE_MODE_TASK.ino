#include "HEADER.h"

// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================

void SAFE_MODE_TASK(void *pvParameters) {
    // Initial delay at the start of the task to allow for system stabilization
    const TickType_t startupDelay = pdMS_TO_TICKS(2000); // 2000 ms delay for startup
    vTaskDelay(startupDelay);

    for (;;) {
        if (configManager.config.TRANSDUCER_1_KPA_SAFE) {
            if (PRESSURE_KPA_1 < configManager.config.B1_MIN_KPA || PRESSURE_KPA_1 > configManager.config.B1_MAX_KPA) {
                ENTER_SAFE_MODE = true; // Set the error flag 

                if (xSemaphoreTake(xMutexCurrentError, portMAX_DELAY) == pdTRUE) {
                    currentError = TRANSDUCER_1_SM;
                    xSemaphoreGive(xMutexCurrentError);
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}