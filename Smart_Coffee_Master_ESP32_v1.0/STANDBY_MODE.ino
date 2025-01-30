#include "HEADER.h"


void STANDBY_MODE(void *pvParameters) {
    bool RELAYS_OFF = false;
    for (;;) {
        // Check for error state at the beginning of each loop iteration
        if (ENTER_SAFE_MODE) {
            vTaskSuspend(NULL);
        }

        if (RESERVOIR_REFILL_REQUIRED) {
            #ifdef PUMP_ON_RES_REFILL
                digitalWrite(PUMP_PIN, LOW);
            #endif
            #ifdef RELAY1_ON_RES_REFILL
                digitalWrite(RLY1_PIN, LOW);
            #endif
            #ifdef RELAY2_ON_RES_REFILL
                digitalWrite(RLY2_PIN, LOW);
            #endif
            #ifdef RELAY3_ON_RES_REFILL
                digitalWrite(RLY3_PIN, LOW);
            #endif
            #ifdef RELAY4_ON_RES_REFILL
                digitalWrite(RLY4_PIN, LOW);
            #endif
            vTaskDelay(pdMS_TO_TICKS(configManager.config.RES_REFILL_TIMER));
            RELAYS_OFF = true;
        }

        if (!RESERVOIR_REFILL_REQUIRED && RELAYS_OFF) {
            // TURN OFF RELAYS
            RELAYS_OFF = false;
            digitalWrite(PUMP_PIN, HIGH);
            digitalWrite(RLY1_PIN, HIGH);
            digitalWrite(RLY2_PIN, HIGH);
            digitalWrite(RLY3_PIN, HIGH);
            digitalWrite(RLY4_PIN, HIGH);
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
