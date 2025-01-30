#include "HEADER.h"

// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================


void BOILER_WTR_TASK(void *pvParameters) {
    for (;;) {
        // Check for error state at the beginning of each loop iteration
        if (ENTER_SAFE_MODE) {
            vTaskSuspend(NULL);
        }

        // Boiler 1
        if (configManager.config.B1_WTR_PROBE) {
            int currentBoiler1Value = touchRead(BOILER_1_PROBE_PIN);
            boiler1Buffer[boiler1Index] = currentBoiler1Value;
            boiler1Index = (boiler1Index + 1) % BUFFER_SIZE;
            BOILER_1_PROBE_TOUCH_VALUE = calculateRunningAverage(boiler1Buffer);
            SP_BOILER_1_PROBE = BOILER_1_PROBE_TOUCH_VALUE; // Update the serial printing variable
        }

        // Boiler 2
        if (configManager.config.B2_WTR_PROBE) {
            int currentBoiler2Value = touchRead(BOILER_2_PROBE_PIN);
            boiler2Buffer[boiler2Index] = currentBoiler2Value;
            boiler2Index = (boiler2Index + 1) % BUFFER_SIZE;
            BOILER_2_PROBE_TOUCH_VALUE = calculateRunningAverage(boiler2Buffer);
            SP_BOILER_2_PROBE = BOILER_2_PROBE_TOUCH_VALUE; // Update the serial printing variable

            if (BOILER_2_PROBE_TOUCH_VALUE > configManager.config.B2_REFILL_TRIG) {
                BOILER_2_REFILL = true;
            } else {
                BOILER_2_REFILL = false;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
