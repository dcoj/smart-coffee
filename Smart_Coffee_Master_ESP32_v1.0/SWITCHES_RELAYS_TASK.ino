#include "HEADER.h"

// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================

void SWITCHES_RELAYS_TASK(void *pvParameters) {
    for (;;) {
        static bool LAST_BREW_SW_PRESSED = false;
        static bool LAST_ES_SW_PRESSED = false;

        // Check for error state at the beginning of each loop iteration
        if (ENTER_SAFE_MODE) {
            // Permanently suspend this task
            vTaskSuspend(NULL);
        }

        // Check for boiler refill
        if (REFILL_IN_PROGRESS) {
            // Pause this task until the flag is cleared
            while (REFILL_IN_PROGRESS) {
                vTaskDelay(pdMS_TO_TICKS(500)); // Wait for 500 ms before checking the flag again
            }
        }



        if (configManager.config.TOGGLE_BREW_SW) {
            // Check for changes in the debounced state for brew switch
            if (BREW_SW_PRESSED != LAST_BREW_SW_PRESSED) {
                if (BREW_SW_PRESSED) {
                    BREW_SW_DEBOUNCE = !BREW_SW_DEBOUNCE;  // Toggle debounced state on press
                    BREW_SW_ON = BREW_SW_DEBOUNCE;  // Update the main flag
                }
                LAST_BREW_SW_PRESSED = BREW_SW_PRESSED;
            }
        } else {
          BREW_SW_ON = !digitalRead(BREW_SW_PIN);
        }


        if (configManager.config.TOGGLE_ES_SW) {
            // Check for changes in the debounced state for espresso mode switch
            if (ES_SW_PRESSED != LAST_ES_SW_PRESSED) {
                if (ES_SW_PRESSED) {
                    ES_SW_DEBOUNCE = !ES_SW_DEBOUNCE;  // Toggle debounced state on press
                    ES_SW_ON = ES_SW_DEBOUNCE;  // Update the main flag
                }
                LAST_ES_SW_PRESSED = ES_SW_PRESSED;
            }
        } else {
          ES_SW_ON = !digitalRead(ES_MODE_PIN);
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}