#include "HEADER.h"

// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================

void PID_B1_ELEMENT_TASK(void *pvParameters) {
    myAutoPIDRelay->setGains(configManager.config.B1_KP, configManager.config.B1_KI, configManager.config.B1_KD);
    myAutoPIDRelay->setTimeStep(100);

    while (1) {
        // SUSPEND TASK IF SAFE MODE IS ACTIVE
        if (ENTER_SAFE_MODE) {
            digitalWrite(ELEMENT_1, HIGH);
            vTaskSuspend(NULL);  // Suspend task in safe mode
        }

        // PAUSE TASK WHEN REFILLING BOILER, RESERVOIR IS LOW, OR CALIBRATION MODE IS ACTIVE
        if (REFILL_IN_PROGRESS || RESERVOIR_REFILL_REQUIRED || CALIBRATING_B1 || configManager.config.DISABLE_RELAYS) {
            // Turn off the heating element for safety
            digitalWrite(ELEMENT_1, HIGH);
            
            // Pause this task until the flag is cleared
            while (REFILL_IN_PROGRESS || RESERVOIR_REFILL_REQUIRED || CALIBRATING_B1 || configManager.config.DISABLE_RELAYS) {
                vTaskDelay(pdMS_TO_TICKS(500)); // Wait for 500 ms before checking the flag again
            }
        } else {
            // PID control loop
            myAutoPIDRelay->run();
            digitalWrite(ELEMENT_1, relayState ? LOW : HIGH);
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // Add a delay to avoid tight looping
    }
}
