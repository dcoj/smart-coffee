#include "HEADER.h"

// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================

void PID_B2_ELEMENT_TASK(void *pvParameters) {
    myAutoPIDRelay->setGains(configManager.config.B2_KP, configManager.config.B2_KI, configManager.config.B2_KD);
    myAutoPIDRelay->setTimeStep(100);
    while (1) {
        // SUSPEND TASK IF SAFE MODE IS ACTIVE
        if (ENTER_SAFE_MODE) {
            digitalWrite(ELEMENT_2, HIGH);
            vTaskSuspend(NULL);  // Suspend task in safe mode
        }

        // PAUSE TASK WHEN REFILLING BOILER, RESERVOIR IS LOW, OR CALIBRATION MODE IS ACTIVE
        if (BOILER_2_REFILL || RESERVOIR_REFILL_REQUIRED || CALIBRATING_B2) {
            // Turn off the heating element for safety
            digitalWrite(ELEMENT_2, HIGH);
        
            // Pause this task until the flag is cleared
            while (BOILER_2_REFILL || RESERVOIR_REFILL_REQUIRED || CALIBRATING_B2) {
                vTaskDelay(pdMS_TO_TICKS(500)); // Wait for 500 ms before checking the flag again
            }
        } else {
            // PID control loop
            myAutoPIDRelay->run();
            digitalWrite(ELEMENT_2, relayState ? LOW : HIGH);
        }
        
        vTaskDelay(pdMS_TO_TICKS(100)); // Add a delay to avoid tight looping
    }
}