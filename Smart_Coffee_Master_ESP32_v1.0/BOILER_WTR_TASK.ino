#include "HEADER.h"
// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================

void BOILER_WTR_TASK(void *pvParameters) {
    bool BREW_SW_ON = false;  // Status of the Brew Switch

    for (;;) {
        // Check for error state at the beginning of each loop iteration
        if (ENTER_SAFE_MODE) {
            vTaskSuspend(NULL);
        }

        // Check if CALIBRATION_MODE is true or if refill is in progress
        if (CALIBRATION_MODE || RESERVOIR_REFILL_REQUIRED) {
            // Pause the task if CALIBRATION_MODE is true
            if (CALIBRATION_MODE) {
                while (CALIBRATION_MODE) {
                    vTaskDelay(pdMS_TO_TICKS(500)); // Wait and check CALIBRATION_MODE
                }
            }
            // Pause the task if a refill is in progress
            if (RESERVOIR_REFILL_REQUIRED) {
                while (RESERVOIR_REFILL_REQUIRED) {
                    vTaskDelay(pdMS_TO_TICKS(500)); // Wait and check RESERVOIR_REFILL_REQUIRED
                }
            }
            continue; // Continue to the next iteration
        }

        // Update the status of the Brew Switch if Pause refill is enabled
        if (configManager.config.PAUSE_REFILL) {
            BREW_SW_ON = !digitalRead(BREW_SW_PIN);  // Assuming active low
        }

        // Check for boiler 1 probe being triggered
        if (configManager.config.B1_WTR_PROBE) {
            if (B1_TOUCH_VALUE > configManager.config.B1_REFILL_TRIG) {
                if (!REFILL_IN_PROGRESS && !BREW_SW_ON) {
                    REFILL_IN_PROGRESS = true;
                    REFILL_B1 = true;
                    digitalWrite(ELEMENT_1, HIGH); // Element 1 OFF

                    if (!configManager.config.DISABLE_RELAYS) {
                        #ifdef PUMP_ON_B1_REFILL
                            digitalWrite(PUMP_PIN, LOW);  // Water pump ON
                        #endif

                        #ifdef RELAY1_ON_B1_REFILL
                            digitalWrite(RLY1_PIN, LOW);  // Relay1 ON
                        #endif

                        #ifdef RELAY2_ON_B1_REFILL
                            digitalWrite(RLY2_PIN, LOW);  // Relay2 ON
                        #endif

                        #ifdef RELAY3_ON_B1_REFILL
                            digitalWrite(RLY3_PIN, LOW);  // Relay3 ON
                        #endif

                        #ifdef RELAY4_ON_B1_REFILL
                            digitalWrite(RLY4_PIN, LOW);  // Relay4 ON
                        #endif
                    }

                    // Wait until the probe is touching water
                    while (B1_TOUCH_VALUE > configManager.config.B1_REFILL_TRIG) {
                        vTaskDelay(pdMS_TO_TICKS(100));  // Small delay to avoid task hogging CPU

                        // Check if CALIBRATION_MODE is active
                        if (CALIBRATION_MODE) {
                            // Exit refill if CALIBRATION_MODE is true
                            REFILL_IN_PROGRESS = false;
                            REFILL_B1 = false;
                            digitalWrite(PUMP_PIN, HIGH);
                            digitalWrite(RLY1_PIN, HIGH);
                            digitalWrite(RLY2_PIN, HIGH);
                            digitalWrite(RLY3_PIN, HIGH);
                            digitalWrite(RLY4_PIN, HIGH);
                            break; // Exit refill loop
                        }
                    }

                    // Start the refill timer once the probe is touching water
                    if (B1_TOUCH_VALUE <= configManager.config.B1_REFILL_TRIG) {
                        vTaskDelay(pdMS_TO_TICKS(configManager.config.B1_REFILL_TIMER));

                        // RELAYS OFF
                        digitalWrite(PUMP_PIN, HIGH);
                        digitalWrite(RLY1_PIN, HIGH);
                        digitalWrite(RLY2_PIN, HIGH);
                        digitalWrite(RLY3_PIN, HIGH);
                        digitalWrite(RLY4_PIN, HIGH);

                        REFILL_IN_PROGRESS = false;
                        REFILL_B1 = false;
                    }
                }
            }
        }

        // Check for boiler 2 probe being triggered
        if (configManager.config.B2_WTR_PROBE) {
            if (B2_TOUCH_VALUE > configManager.config.B2_REFILL_TRIG) {
                if (!REFILL_IN_PROGRESS && !BREW_SW_ON) {
                    REFILL_IN_PROGRESS = true;
                    digitalWrite(ELEMENT_1, HIGH);  // Element 1 OFF

                    if (!configManager.config.DISABLE_RELAYS) {
                        #ifdef PUMP_ON_B2_REFILL
                            digitalWrite(PUMP_PIN, LOW);  // Water pump ON
                        #endif

                        #ifdef RELAY1_ON_B2_REFILL
                            digitalWrite(RLY1_PIN, LOW);  // Relay1 ON
                        #endif

                        #ifdef RELAY2_ON_B2_REFILL
                            digitalWrite(RLY2_PIN, LOW);  // Relay2 ON
                        #endif

                        #ifdef RELAY3_ON_B2_REFILL
                            digitalWrite(RLY3_PIN, LOW);  // Relay3 ON
                        #endif

                        #ifdef RELAY4_ON_B2_REFILL
                            digitalWrite(RLY4_PIN, LOW);  // Relay4 ON
                        #endif
                    }

                    // Wait until the probe is touching water
                    while (B2_TOUCH_VALUE < configManager.config.B2_REFILL_TRIG) {
                        vTaskDelay(pdMS_TO_TICKS(100));  // Small delay to avoid task hogging CPU

                        // Check if CALIBRATION_MODE is active
                        if (CALIBRATION_MODE) {
                            // Exit refill if CALIBRATION_MODE is true
                            REFILL_IN_PROGRESS = false;
                            digitalWrite(PUMP_PIN, HIGH);
                            digitalWrite(RLY1_PIN, HIGH);
                            digitalWrite(RLY2_PIN, HIGH);
                            digitalWrite(RLY3_PIN, HIGH);
                            digitalWrite(RLY4_PIN, HIGH);
                            break; // Exit refill loop
                        }
                    }

                    // Start the refill timer once the probe is touching water
                    if (B2_TOUCH_VALUE >= configManager.config.B2_REFILL_TRIG) {
                        vTaskDelay(pdMS_TO_TICKS(configManager.config.B2_REFILL_TIMER));

                        // RELAYS OFF
                        digitalWrite(PUMP_PIN, HIGH);
                        digitalWrite(RLY1_PIN, HIGH);
                        digitalWrite(RLY2_PIN, HIGH);
                        digitalWrite(RLY3_PIN, HIGH);
                        digitalWrite(RLY4_PIN, HIGH);

                        REFILL_IN_PROGRESS = false;
                    }
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay to avoid tight looping
    }
}

