#include "HEADER.h"

// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================

void LOAD_CELL_TASK(void *pvParameters) {
    for (;;) {
        // Check the flag at the beginning of each loop iteration
        if (CALIBRATING_LOAD_CELL) {
            
            // Pause this task until the flag is cleared
            while (CALIBRATING_LOAD_CELL) {
                vTaskDelay(pdMS_TO_TICKS(100)); // Wait for 100 ms before checking the flag again
            }
        }
        float weight = scale.get_units(2); // Average of 5 readings

        if (xSemaphoreTake(xMutexWeight, (TickType_t) 10) == pdTRUE) {
            LOAD_CELL_WEIGHT = weight;
            xSemaphoreGive(xMutexWeight);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}