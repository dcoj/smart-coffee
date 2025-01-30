#include "HEADER.h"

// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================

void SAFE_MODE_TASK(void *pvParameters) {
    // Initial delay at the start of the task to allow for system stabilization
    const TickType_t startupDelay = pdMS_TO_TICKS(2000); // 2000 ms delay for startup
    vTaskDelay(startupDelay);
    
    for (;;) {
        // TRANSDUCER 2 SAFE-MODE CHECK
        if (configManager.config.TRANSDUCER_2_KPA_SAFE) {
            if (PRESSURE_KPA_2 < configManager.config.B2_MIN_KPA || PRESSURE_KPA_2 > configManager.config.B2_MAX_KPA) {
                // SEND UART MESSAGE TO TRIGGER SAFE MODE
                uart_write_bytes(UART_NUM_1, "PT2_P_SM\n", strlen("B2_P_SM\n"));
                Serial.println("SENT BOILER 2 PRESSURE SAFE MODE VIA UART");
                ENTER_SAFE_MODE = true; // Set the error flag 

                if (xSemaphoreTake(xMutexCurrentError, portMAX_DELAY) == pdTRUE) {
                    currentError = PRESSURE_SENSOR_OUT_OF_RANGE_B2;
                    xSemaphoreGive(xMutexCurrentError);
                }
            }
        }

        // TEMP SENSOR 1 SAFE-MODE CHECK
        if (configManager.config.TEMP_1_SAFE) {
            if (TEMP_C_1 < configManager.config.TEMP_1_MIN || TEMP_C_1 > configManager.config.TEMP_1_MAX) {
                // SEND UART MESSAGE TO TRIGGER SAFE MODE
                uart_write_bytes(UART_NUM_1, "TS_1_SM\n", strlen("TS_1_SM\n"));
                Serial.println("TEMP SENSOR 1 TRIGGERED SAFE MODE!");
                ENTER_SAFE_MODE = true; // Set the error flag

                if (xSemaphoreTake(xMutexCurrentError, portMAX_DELAY) == pdTRUE) {
                    currentError = TEMP_SENSOR_1_SM; 
                    xSemaphoreGive(xMutexCurrentError);
                }
            }
        }

        // TEMP SENSOR 2 SAFE-MODE CHECK
        if (configManager.config.TEMP_2_SAFE) {
            if (TEMP_C_2 < configManager.config.TEMP_2_MIN || TEMP_C_2 > configManager.config.TEMP_2_MAX) {
                // SEND UART MESSAGE TO TRIGGER SAFE MODE
                uart_write_bytes(UART_NUM_1, "TS_2_SM\n", strlen("TS_2_SM\n"));
                Serial.println("TEMP SENSOR 2 TRIGGERED SAFE MODE!");
                ENTER_SAFE_MODE = true; // Set the error flag

                if (xSemaphoreTake(xMutexCurrentError, portMAX_DELAY) == pdTRUE) {
                    currentError = TEMP_SENSOR_2_SM; 
                    xSemaphoreGive(xMutexCurrentError);
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}