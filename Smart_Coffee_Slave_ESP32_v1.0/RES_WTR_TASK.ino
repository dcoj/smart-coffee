#include "HEADER.h"

// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================

void RES_WTR_TASK(void *pvParameters) {

    for (;;) {
        // Check for error state at the beginning of each loop iteration
        if (ENTER_SAFE_MODE) {
            // Permanently suspend this task
            vTaskSuspend(NULL);
        }

        if (configManager.config.WTR_RES_PROBE) {
            // Touch sensitive reservoir probe
            RES_PROBE_TOUCH_VALUE = touchRead(RESERVOIR_PIN);
            SP_RES_PROBE = RES_PROBE_TOUCH_VALUE; // Update the serial printing variable


            if (RES_PROBE_TOUCH_VALUE > configManager.config.RES_PROBE) {
                if (!RESERVOIR_REFILL_REQUIRED) {
                    RESERVOIR_REFILL_REQUIRED = true; // Setting the flag to true
                    RESERVOIR_REFILL_REQUIRED_UART = true; // Setting the flag to true
                    // Turn off relays
                    digitalWrite(ELEMENT_2, HIGH);
                }
            } else {
                if (RESERVOIR_REFILL_REQUIRED) {
                    RESERVOIR_REFILL_REQUIRED = false;
                    RESERVOIR_FULL_UART = true;
                }
            }
            
        }

        
        // Ultrasonic level detection
        if (configManager.config.US_LVL_DETECT) {
          if (ULTRA_WATER_PERCENTAGE < configManager.config.RES_LOW) {
              if (!RESERVOIR_REFILL_REQUIRED) {
                  Serial.print("Setting res water to EMPTY");
                  RESERVOIR_REFILL_REQUIRED = true; // Setting the flag to true
                  RESERVOIR_REFILL_REQUIRED_UART = true; // Setting the flag to true
                  // Turn off relays
                  digitalWrite(ELEMENT_2, HIGH);
              }
          } else {
              if (RESERVOIR_REFILL_REQUIRED) {
                  Serial.print("Setting res water to FULL");
                  RESERVOIR_REFILL_REQUIRED = false;
                  RESERVOIR_FULL_UART = true;
              }
          }
        }

        // Reservoir Switch logic
        if (configManager.config.RES_SW) {
            if (configManager.config.RES_SW_NC) {
                // Switch is normally closed
                if (digitalRead(RESERVOIR_PIN) == HIGH) {
                    // Reservoir is empty
                    if (!RESERVOIR_REFILL_REQUIRED) {
                        RESERVOIR_REFILL_REQUIRED = true;
                        RESERVOIR_REFILL_REQUIRED_UART = true;
                        // Turn off relays
                        digitalWrite(ELEMENT_2, HIGH);
                    }
                } else {
                    // Reservoir is full
                    if (RESERVOIR_REFILL_REQUIRED) {
                        RESERVOIR_REFILL_REQUIRED = false;
                        RESERVOIR_FULL_UART = true;
                    }
                }
            } else {
                // Switch is normally open
                if (digitalRead(RESERVOIR_PIN) == LOW) {
                    // Reservoir is empty
                    if (!RESERVOIR_REFILL_REQUIRED) {
                        RESERVOIR_REFILL_REQUIRED = true;
                        RESERVOIR_REFILL_REQUIRED_UART = true;
                        // Turn off relays
                        digitalWrite(ELEMENT_2, HIGH);
                    }
                } else {
                    // Reservoir is full
                    if (RESERVOIR_REFILL_REQUIRED) {
                        RESERVOIR_REFILL_REQUIRED = false;
                        RESERVOIR_FULL_UART = true;
                    }
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}