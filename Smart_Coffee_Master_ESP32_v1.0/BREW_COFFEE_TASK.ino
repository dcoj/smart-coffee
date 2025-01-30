#include "HEADER.h"

// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================

void BREW_COFFEE_TASK(void *pvParameters) {
    for (;;) {
        static unsigned long PRE_INFUS_TIMER = 0;
        static unsigned long BREW_TIMER = 0; 

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
        // Check if the reservoir needs refilling
        if (RESERVOIR_REFILL_REQUIRED || CALIBRATION_MODE) {
            // Pause this task until the flag is cleared
            digitalWrite(PUMP_PIN, HIGH);  // Water pump off
            while (RESERVOIR_REFILL_REQUIRED || CALIBRATION_MODE) {
                vTaskDelay(pdMS_TO_TICKS(500)); // Wait for 500 ms before checking the flag again
            }
        }
        
        if (BREW_SW_ON) {
            if (ES_SW_ON) {
                switch (currentShotState) {
                    case SHOT_IDLE:
                        if (configManager.config.LOAD_CELL) {
                            // Tare the scale
                            scale.tare();
                        }
                        
    if (configManager.config.PRE_INFUSION) {
                            currentShotState = SHOT_PRIME;
    } else {
                            currentShotState = SHOT_BREWING;
                            BREW_TIMER = millis();
    }
                            shotStartTime = millis();
                            preInfusionCycleCount = 0;
                            preInfusionState = PRE_INFUSION_OFF;
                            break;

    if (configManager.config.PRE_INFUSION) {
                    case SHOT_PRIME:
                        if (!configManager.config.DISABLE_RELAYS) {
                            #ifdef PUMP_ON_DURING_BREW
                                digitalWrite(PUMP_PIN, LOW);
                            #endif

                            #ifdef RELAY1_ON_DURING_BREW
                                digitalWrite(RLY1_PIN, LOW);
                            #endif

                            #ifdef RELAY2_ON_DURING_BREW
                                digitalWrite(RLY2_PIN, LOW);
                            #endif

                            #ifdef RELAY3_ON_DURING_BREW
                                digitalWrite(RLY3_PIN, LOW);
                            #endif

                            #ifdef RELAY4_ON_DURING_BREW
                                digitalWrite(RLY4_PIN, LOW);
                            #endif

                            #ifdef RELAY1_ON_END_BREW
                                digitalWrite(RLY1_PIN, HIGH);
                            #endif

                            #ifdef RELAY2_ON_END_BREW
                                digitalWrite(RLY2_PIN, HIGH);
                            #endif

                            #ifdef RELAY3_ON_END_BREW
                                digitalWrite(RLY3_PIN, HIGH);
                            #endif

                            #ifdef RELAY4_ON_END_BREW
                                digitalWrite(RLY4_PIN, HIGH);
                            #endif
                        }

                        if (millis() - shotStartTime >= configManager.config.PRIME_PUMP) {
                            currentShotState = SHOT_PRE_INFUSION;
                            PRE_INFUS_TIMER = millis();
                            preInfusionCycleCount = 0;
                        }
                        break;

                    case SHOT_PRE_INFUSION:
                        if (millis() - PRE_INFUS_TIMER < configManager.config.INFUSION_TIMER) {
                            unsigned long cyclePhase = (millis() - PRE_INFUS_TIMER) % (2 * PRE_INFUSION_CYCLE_TIME);
                            if (cyclePhase < PRE_INFUSION_CYCLE_TIME) {
                                if (!configManager.config.DISABLE_RELAYS) {
                                    #ifdef PUMP_ON_DURING_BREW
                                        digitalWrite(PUMP_PIN, HIGH);
                                    #endif
                                }
                                preInfusionState = PRE_INFUSION_ON;
                            } else {
                                if (!configManager.config.DISABLE_RELAYS) {
                                    #ifdef PUMP_ON_DURING_BREW
                                        digitalWrite(PUMP_PIN, LOW);
                                    #endif
                                }
                                preInfusionState = PRE_INFUSION_OFF;
                                preInfusionCycleCount++;
                            }
                        } else {
                            preInfusionState = PRE_INFUSION_OFF;
                            currentShotState = SHOT_BREWING;
                            BREW_TIMER = millis();
                        }
                        break;
    }
                    case SHOT_BREWING:
                        {
                        // ESPRESSO WEIGHT TARGET
                        if (configManager.config.ESPRESSO_WEIGHT) {
                            float currentWeight = 0.0;
                            if (xSemaphoreTake(xMutexWeight, (TickType_t) 10) == pdTRUE) {
                                currentWeight = LOAD_CELL_WEIGHT;
                                xSemaphoreGive(xMutexWeight);
                            }

                            // Check for weight target
                            if (currentWeight < configManager.config.SHOT_GRAM - configManager.config.WEIGHT_OFFSET) {
                                if (!configManager.config.DISABLE_RELAYS) {
                                    #ifdef PUMP_ON_DURING_BREW
                                        digitalWrite(PUMP_PIN, LOW);
                                    #endif
                                }
                            } else {
                                if (!configManager.config.DISABLE_RELAYS) {
                                    #ifdef PUMP_ON_DURING_BREW
                                        digitalWrite(PUMP_PIN, HIGH);
                                    #endif
                                }
                                currentShotState = SHOT_FINISHED;
                            }
                        }

                        // ESPRESSO VOLUME TARGET
                        if (configManager.config.ESPRESSO_VOLUME) {
                            // Flow-based pump control
                            float LOCAL_DISPENSED_WATER = 0.0;
                            if (xSemaphoreTake(xMutexDispensedWater, (TickType_t) 10) == pdTRUE) {
                                LOCAL_DISPENSED_WATER = DISPENSED_WATER;
                                xSemaphoreGive(xMutexDispensedWater);
                            }
                            if (LOCAL_DISPENSED_WATER < configManager.config.SHOT_ML) {
                                if (!configManager.config.DISABLE_RELAYS) {
                                    #ifdef PUMP_ON_DURING_BREW
                                        digitalWrite(PUMP_PIN, LOW);
                                    #endif
                                }
                            } else {
                                if (!configManager.config.DISABLE_RELAYS) {
                                    #ifdef PUMP_ON_DURING_BREW
                                        digitalWrite(PUMP_PIN, HIGH);
                                    #endif
                                }
                                currentShotState = SHOT_FINISHED;
                            }
                        }

                        // ESPRESSO TIMER TARGET
                        if (configManager.config.ESPRESSO_TIMER) {
                            if (millis() - BREW_TIMER < configManager.config.SHOT_TIMER) {
                                if (!configManager.config.DISABLE_RELAYS) {
                                    #ifdef PUMP_ON_DURING_BREW
                                        digitalWrite(PUMP_PIN, LOW);
                                    #endif
                                }
                            } else {
                                if (!configManager.config.DISABLE_RELAYS) {
                                    #ifdef PUMP_ON_DURING_BREW
                                        digitalWrite(PUMP_PIN, HIGH);
                                    #endif
                                }
                                currentShotState = SHOT_FINISHED;
                            }
                        }
                        }
                        break;

                    case SHOT_FINISHED:
                        if (!configManager.config.DISABLE_RELAYS) {
                            #ifdef PUMP_ON_DURING_BREW
                                digitalWrite(PUMP_PIN, HIGH);
                            #endif
                            #ifdef RELAY1_ON_DURING_BREW
                                digitalWrite(RLY1_PIN, HIGH);
                            #endif

                            #ifdef RELAY2_ON_DURING_BREW
                                digitalWrite(RLY2_PIN, HIGH);
                            #endif

                            #ifdef RELAY3_ON_DURING_BREW
                                digitalWrite(RLY3_PIN, HIGH);
                            #endif

                            #ifdef RELAY4_ON_DURING_BREW
                                digitalWrite(RLY4_PIN, HIGH);
                            #endif

                            #ifdef RELAY1_ON_END_BREW
                                digitalWrite(RLY1_PIN, LOW);
                            #endif

                            #ifdef RELAY2_ON_END_BREW
                                digitalWrite(RLY2_PIN, LOW);
                            #endif

                            #ifdef RELAY3_ON_END_BREW
                                digitalWrite(RLY3_PIN, LOW);
                            #endif

                            #ifdef RELAY4_ON_END_BREW
                                digitalWrite(RLY4_PIN, LOW);
                            #endif
                        }
                        if (configManager.config.TOGGLE_BREW_SW) {
                            if (BREW_SW_DEBOUNCE) {
                                BREW_SW_DEBOUNCE = false;
                            } else {
                                BREW_SW_DEBOUNCE = true;
                            }
                            BREW_SW_ON = false;
                            currentShotState = SHOT_IDLE;
                        }

                        break;
                }
            } else {
                if (!configManager.config.DISABLE_RELAYS) {
                    #ifdef PUMP_ON_DURING_BREW
                        digitalWrite(PUMP_PIN, LOW);
                    #endif
                    #ifdef RELAY1_ON_DURING_BREW
                        digitalWrite(RLY1_PIN, LOW);
                    #endif

                    #ifdef RELAY2_ON_DURING_BREW
                        digitalWrite(RLY2_PIN, LOW);
                    #endif

                    #ifdef RELAY3_ON_DURING_BREW
                        digitalWrite(RLY3_PIN, LOW);
                    #endif

                    #ifdef RELAY4_ON_DURING_BREW
                        digitalWrite(RLY4_PIN, LOW);
                    #endif

                    #ifdef RELAY1_ON_END_BREW
                        digitalWrite(RLY1_PIN, HIGH);
                    #endif

                    #ifdef RELAY2_ON_END_BREW
                        digitalWrite(RLY2_PIN, HIGH);
                    #endif

                    #ifdef RELAY3_ON_END_BREW
                        digitalWrite(RLY3_PIN, HIGH);
                    #endif

                    #ifdef RELAY4_ON_END_BREW
                        digitalWrite(RLY4_PIN, HIGH);
                    #endif
                }
            }
        } else {
            if (!configManager.config.DISABLE_RELAYS) {
                #ifdef PUMP_ON_DURING_BREW
                    digitalWrite(PUMP_PIN, HIGH);
                #endif
                #ifdef RELAY1_ON_DURING_BREW
                    digitalWrite(RLY1_PIN, HIGH);
                #endif

                #ifdef RELAY2_ON_DURING_BREW
                    digitalWrite(RLY2_PIN, HIGH);
                #endif

                #ifdef RELAY3_ON_DURING_BREW
                    digitalWrite(RLY3_PIN, HIGH);
                #endif

                #ifdef RELAY4_ON_DURING_BREW
                    digitalWrite(RLY4_PIN, HIGH);
                #endif

                #ifdef RELAY1_ON_END_BREW
                    digitalWrite(RLY1_PIN, LOW);
                #endif

                #ifdef RELAY2_ON_END_BREW
                    digitalWrite(RLY2_PIN, LOW);
                #endif

                #ifdef RELAY3_ON_END_BREW
                    digitalWrite(RLY3_PIN, LOW);
                #endif

                #ifdef RELAY4_ON_END_BREW
                    digitalWrite(RLY4_PIN, LOW);
                #endif
            }
            
            // RESET FLOW METER COUNTER
            if (configManager.config.FLOW_METER) {
                if (xSemaphoreTake(xMutexDispensedWater, (TickType_t)10) == pdTRUE) {
                    DISPENSED_WATER = 0; // Reset dispensed water to 0
                    xSemaphoreGive(xMutexDispensedWater);
                }
            }
            currentShotState = SHOT_IDLE;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}