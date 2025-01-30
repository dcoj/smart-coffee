#include "HEADER.h"

// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================

void PWM_B2_ELEMENT_TASK(void *pvParameters) {
    static unsigned long lastPulseTime = 0;
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
        }

        // PWM logic to control the heating element
        unsigned long currentMillis = millis();
        double distanceToTarget;

        // Calculate distance to target based on whether we're targeting temperature or pressure
        if (configManager.config.TARGET_TEMP_B2) {
            distanceToTarget = configManager.config.B2_TEMP - TEMP_C_2;
        } else {
            distanceToTarget = configManager.config.B2_KPA - PRESSURE_KPA_2;
        }

        // Implement PWM control based on the distance to target
        if (distanceToTarget < -configManager.config.B2_STABLE_TH) {
            digitalWrite(ELEMENT_2, HIGH);
        } else if (distanceToTarget < 0) {
            if (currentMillis - lastPulseTime >= configManager.config.B2_STABLE_PWM) {
                lastPulseTime = currentMillis;
                digitalWrite(ELEMENT_2, LOW);
            } else if (currentMillis - lastPulseTime < configManager.config.B2_PWR) {
                digitalWrite(ELEMENT_2, LOW);
            } else {
                digitalWrite(ELEMENT_2, HIGH);
            }
        } else if (fabs(distanceToTarget) > configManager.config.B2_PWM) {
            digitalWrite(ELEMENT_2, LOW);
        } else {
            unsigned long POWER_OFF = map(distanceToTarget, 0, configManager.config.B2_PWM, configManager.config.B2_MAX_OT, configManager.config.B2_MIN_OT);
            if (currentMillis - lastPulseTime >= POWER_OFF) {
                lastPulseTime = currentMillis;
                digitalWrite(ELEMENT_2, LOW);
            } else if (currentMillis - lastPulseTime < configManager.config.B2_PWR) {
                digitalWrite(ELEMENT_2, LOW);
            } else {
                digitalWrite(ELEMENT_2, HIGH);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
