#include "HEADER.h"

// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================

// ==============================================
// ========= DUAL BOILER DISPLAY LAYOUT =========
// ==============================================
void DUAL_BOILER_DISPLAY_TASK(void *pvParameters) {
    bool STARTUP_LOGO = true; // Initialize the flag
    for (;;) {
        if (configManager.config.STARTUP_LOGO && STARTUP_LOGO) {
            display.clearDisplay();
            display.setTextColor(SH110X_WHITE);
            display.setTextSize(3);
            display.setCursor(30, 105);
            display.print("v1.0");
            display.drawBitmap(0, 0, logo, 128, 90, SH110X_WHITE);
            display.display();
            
            // Wait for 3 seconds (non-blocking)
            vTaskDelay(pdMS_TO_TICKS(3000));
            STARTUP_LOGO = false;
            display.clearDisplay(); // Clear display after showing the logo
        } else {
            // Error handling
            if (xSemaphoreTake(xMutexCurrentError, portMAX_DELAY) == pdTRUE) {
                if (currentError != NO_ERROR) {
                    // Error handling display logic
                    display.clearDisplay();
                    display.setTextSize(2);      
                    display.setTextColor(SH110X_WHITE);
                    display.setCursor(0, 0);
                    switch(currentError) {
                        case TRANSDUCER_1_SM:
                            display.print("SAFE MODE\n\nError:\nTransducer 1\nPressure\nOut of limits");
                            break;
                        case TRANSDUCER_2_SM:
                            display.print("SAFE MODE\n\nError:\nTransducer 2\nPressure\nOut of limits");
                            break;
                        case TEMP_SENSOR_1_SM:
                            display.print("SAFE MODE\n\nError:\nTemp Sensor1\nOut of limits");
                            break;
                        case TEMP_SENSOR_2_SM:
                            display.print("SAFE MODE\n\nError:\nTemp Sensor2\nOut of limits");
                            break;
                        case LOST_UART_CONNECTION:
                            display.print("SAFE MODE\n\nError:\nUART\nConnection\nLost");
                            break;
                        default:
                            break;
                    }
                    display.display();
                    // Release the currentError mutex after use
                    xSemaphoreGive(xMutexCurrentError);
                    vTaskDelay(pdMS_TO_TICKS(100));  // Update frequency
                    continue; // Continue the loop, skipping regular display code
                }
                // Release the currentError mutex after use
                xSemaphoreGive(xMutexCurrentError);

                // Regular display logic
                
                // DISPLAY REFILL RESERVOIR MESSAGE
                if (RESERVOIR_REFILL_REQUIRED) {
                    display.clearDisplay();
                    display.drawBitmap(0, 0, REFILL_RESERVOIR, 128, 128, 1);
                    display.display();
                // DISPLAY REFILL BOILER BITMAP
                } else if (REFILL_IN_PROGRESS) {
                    display.clearDisplay();
                    display.drawBitmap(0, 0, REFILL_BOILER, 128, 128, 1);
                    display.display();
                // DISPLAY CALIBRATION MODE ICON
                } else if (CALIBRATION_MODE_ICON) {
                    display.clearDisplay();
                    display.drawBitmap(0, 0, CALIBRATION_ICON, 128, 128, 1);
                    display.display();
                } else {
                    display.setTextSize(2);

                    // DISPLAY REGULAR INFO
                    float LOCAL_PRESSURE_KPA_1 = 0.0;
                    float LOCAL_PRESSURE_KPA_2 = 0.0;
                    double LOCAL_TEMP_C_1 = 0.0;
                    double LOCAL_TEMP_C_2 = 0.0;
                    int LOCAL_WATER_PERCENTAGE = 100;
                    float LOCAL_LOAD_CELL_WEIGHT = 0.0;

                    if (configManager.config.LOAD_CELL) {
                        if (xSemaphoreTake(xMutexWeight, portMAX_DELAY) == pdTRUE) {
                            LOCAL_LOAD_CELL_WEIGHT = LOAD_CELL_WEIGHT;
                            xSemaphoreGive(xMutexWeight);
                        }
                    }
                    
                    if (xSemaphoreTake(xMutexPressure, portMAX_DELAY) == pdTRUE) {
                        LOCAL_PRESSURE_KPA_1 = PRESSURE_KPA_1;
                        xSemaphoreGive(xMutexPressure);
                    }

                    if (xSemaphoreTake(xMutexPressure2, portMAX_DELAY) == pdTRUE) {
                        LOCAL_PRESSURE_KPA_2 = PRESSURE_KPA_2;
                        xSemaphoreGive(xMutexPressure2);
                    }

                    if (xSemaphoreTake(xMutexWaterResPercentage, portMAX_DELAY) == pdTRUE) {
                        LOCAL_WATER_PERCENTAGE = ULTRA_WATER_PERCENTAGE;
                        xSemaphoreGive(xMutexWaterResPercentage);
                    }

                    if (xSemaphoreTake(xMutexTemprature1, portMAX_DELAY) == pdTRUE) {
                        LOCAL_TEMP_C_1 = TEMP_C_1;
                        xSemaphoreGive(xMutexTemprature1);
                    }

                    if (xSemaphoreTake(xMutexTemprature2, portMAX_DELAY) == pdTRUE) {
                        LOCAL_TEMP_C_2 = TEMP_C_2;
                        xSemaphoreGive(xMutexTemprature2);
                    }
                    
                    display.clearDisplay();

                    // Display kPa or PSI
                    if (configManager.config.USE_IMPERIAL_UNITS) {
                        display.setTextSize(1);
                        display.setCursor(50, 40);
                        display.print("PSI");
                        display.setTextSize(2);
                    } else {
                        display.setTextSize(1);
                        display.setCursor(50, 40);
                        display.print("kPa");
                        display.setTextSize(2);
                    }

                    // Display Pressure 1
                    if (configManager.config.TRANSDUCER_1) {
                        display.setCursor(3, 32);
                        if (configManager.config.USE_IMPERIAL_UNITS) {
                            float PT1_PSI = LOCAL_PRESSURE_KPA_1 * 0.14503773779;
                            int CONSTRAINED_PT1_PSI = PT1_PSI < (5 * 0.14503773779) ? 0 : max(0, (int)PT1_PSI);
                            display.print(CONSTRAINED_PT1_PSI);
                        } else {
                            int CONSTRAINED_PRESSURE_KPA_1 = LOCAL_PRESSURE_KPA_1 < 5 ? 0 : max(0, (int)LOCAL_PRESSURE_KPA_1);
                            display.print(CONSTRAINED_PRESSURE_KPA_1);
                        }
                    }

                    // Display Pressure 2
                    if (configManager.config.TRANSDUCER_2) {
                        display.setCursor(90, 32);
                        if (configManager.config.USE_IMPERIAL_UNITS) {
                            float PT2_PSI = LOCAL_PRESSURE_KPA_2 * 0.14503773779;
                            int CONSTRAINED_PT2_PSI = PT2_PSI < (5 * 0.14503773779) ? 0 : max(0, (int)PT2_PSI);
                            display.print(CONSTRAINED_PT2_PSI);
                        } else {
                            int CONSTRAINED_PRESSURE_KPA_2 = LOCAL_PRESSURE_KPA_2 < 5 ? 0 : max(0, (int)LOCAL_PRESSURE_KPA_2);
                            display.print(CONSTRAINED_PRESSURE_KPA_2);
                        }
                    }

                    // Display "C" or "F"
                    if (configManager.config.USE_IMPERIAL_UNITS) {
                        display.setTextSize(1);
                        display.setCursor(50, 68);
                        display.print(" F");
                        display.setTextSize(2);
                    } else {
                        display.setTextSize(1);
                        display.setCursor(50, 68);
                        display.print(" C");
                        display.setTextSize(2);
                    }

                    // Display TEMP 1
                    if (configManager.config.TEMP_SENSOR_1) {
                        display.setCursor(3, 59);
                        if (configManager.config.USE_IMPERIAL_UNITS) {
                            float TEMP_1_F = (LOCAL_TEMP_C_1 * 9/5) + 32;
                            display.print((int)TEMP_1_F);
                        } else {
                            display.print(LOCAL_TEMP_C_1, 0);  // Display temperature with zero decimal place
                        }
                    }

                    // Display TEMP 2
                    if (configManager.config.TEMP_SENSOR_2) {
                        display.setCursor(90, 59);
                        if (configManager.config.USE_IMPERIAL_UNITS) {
                            float TEMP_2_F = (LOCAL_TEMP_C_2 * 9/5) + 32;
                            display.print((int)TEMP_2_F);
                        } else {
                            display.print(LOCAL_TEMP_C_2, 0);  // Display temperature with zero decimal place
                        }
                    }

                    // Draw horizontal bar to devide screen
                    display.drawBitmap(0, 26, Horizontal_bar, 128, 1, 1);

                    display.drawBitmap(0, 53, Horizontal_bar, 128, 1, 1);

                    display.drawBitmap(0, 80, Horizontal_bar, 128, 1, 1);

                    // Display the weight
                    if (configManager.config.LOAD_CELL) {
                        float CONSTRAINED_LOAD_CELL_WEIGHT = max(0.0f, LOCAL_LOAD_CELL_WEIGHT);
                        display.setCursor(0, 108);
                        if (configManager.config.USE_IMPERIAL_UNITS) {
                            // Convert grams to ounces (1 gram = 0.035274 ounces)
                            float LOAD_CELL_OZ = CONSTRAINED_LOAD_CELL_WEIGHT * 0.035274;
                            display.print(LOAD_CELL_OZ, 1);  // Display weight in ounces with one decimal place
                            display.setTextSize(1);
                            display.print(" oz");
                        } else {
                            display.print(CONSTRAINED_LOAD_CELL_WEIGHT, 0);  // Display weight in grams with no decimal places
                            display.setTextSize(1);
                            display.print(" g");
                        }
                    }

                    // Display the dispensed water value
                    if (configManager.config.FLOW_METER) {
                        display.setCursor(0, 108);
                        if (configManager.config.USE_IMPERIAL_UNITS) {
                            float flowInOz = DISPENSED_WATER * 0.033814 / 10.0;
                            display.print(flowInOz, 0);
                            display.setTextSize(1);
                            display.setCursor(55, 110);
                            display.print("oz");
                            display.setTextSize(2);;
                        } else {
                            display.print(DISPENSED_WATER, 0);
                            display.setTextSize(1);
                            display.print(" ml");
                            display.setTextSize(2);
                        }
                    }


                    // Display ultra sonic water percentage
                    if (configManager.config.ULTRASONIC_SENSOR) {
                        display.drawBitmap(90, 85, Waterdrops, 20, 20, 1);
                        display.setTextSize(2);
                        display.setCursor(90, 110);
                        //display.print(LOCAL_WATER_PERCENTAGE);
                        int CONSTRAIN_LOCAL_WATER_PERCENTAGE = constrain(LOCAL_WATER_PERCENTAGE, 0, 99);
                        display.print(CONSTRAIN_LOCAL_WATER_PERCENTAGE);
                        display.setTextSize(1);
                        display.print(" %");
                        display.setTextSize(2);
                    }

                    if (ES_SW_ON) {
                        display.drawBitmap(0, 79, EspressoMode, 64, 20, 1);
                    }



                    // Display element 1 state by flashing the water boiler icon
                    if (digitalRead(ELEMENT_1) == LOW) {
                        display.drawBitmap(00, 0, HeatWaves24px, 24, 24, 1);
                    }

                    // Display pump icon
                    if (digitalRead(PUMP_PIN) == LOW) {
                        display.drawBitmap(52, 2, Pump_24px, 24, 24, 1);
                    }

                    // Display element 2 state by flashing the steam boiler icon
                    if (ELEMENT_2_STATE) {
                        display.drawBitmap(104, 0, HeatWaves24px, 24, 24, 1);
                    }

                    display.display();
                }

                vTaskDelay(pdMS_TO_TICKS(20));  // Update frequency
            }
        }
    }
}
