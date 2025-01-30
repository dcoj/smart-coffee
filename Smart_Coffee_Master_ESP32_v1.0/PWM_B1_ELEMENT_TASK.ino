#include "HEADER.h"

// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================

void PWM_B1_ELEMENT_TASK(void *pvParameters) {
    static unsigned long lastPulseTime = 0;
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
        }

        unsigned long currentMillis = millis();
        double distanceToTarget;

        // Calculate distance to target based on whether we're targeting temperature or pressure
        if (configManager.config.TARGET_TEMP_B1) {
            distanceToTarget = configManager.config.B1_TEMP - TEMP_C_1;
        } else {
            distanceToTarget = configManager.config.B1_KPA - PRESSURE_KPA_1;
        }

        // Implement PWM control based on the distance to target
        if (distanceToTarget < -configManager.config.B1_STABLE_TH) {
            digitalWrite(ELEMENT_1, HIGH);
        } else if (distanceToTarget < 0) {
            if (currentMillis - lastPulseTime >= configManager.config.B1_STABLE_PWM) {
                lastPulseTime = currentMillis;
                digitalWrite(ELEMENT_1, LOW);
            } else if (currentMillis - lastPulseTime < configManager.config.B1_PWR) {
                digitalWrite(ELEMENT_1, LOW);
            } else {
                digitalWrite(ELEMENT_1, HIGH);
            }
        } else if (fabs(distanceToTarget) > configManager.config.B1_PWM) {
            digitalWrite(ELEMENT_1, LOW);
        } else {
            unsigned long POWER_OFF = map(distanceToTarget, 0, configManager.config.B1_PWM, configManager.config.B1_MAX_OT, configManager.config.B1_MIN_OT);
            if (currentMillis - lastPulseTime >= POWER_OFF) {
                lastPulseTime = currentMillis;
                digitalWrite(ELEMENT_1, LOW);
            } else if (currentMillis - lastPulseTime < configManager.config.B1_PWR) {
                digitalWrite(ELEMENT_1, LOW);
            } else {
                digitalWrite(ELEMENT_1, HIGH);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}