#include "HEADER.h"

// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================

//====================================================
//========== CALIBRATION MODE ICON DISPLAY ===========
//====================================================
void CalibrationModeTask(void *pvParameters) {
    const TickType_t SHOW_ICON = pdMS_TO_TICKS(1000); // 1 second
    const TickType_t HIDE_ICON = pdMS_TO_TICKS(4000); // 4 seconds

    while (true) {
        // Set the flag to true
        CALIBRATION_MODE_ICON = true; // DISPLAY THE ICON
        vTaskDelay(SHOW_ICON);

        // Set the flag to false
        CALIBRATION_MODE_ICON = false; // HIDE THE ICON
        vTaskDelay(HIDE_ICON);
    }
}

//====================================================
//========== CALIBRATION FOR BOILER 1 PROBE ==========
//====================================================
void CALIBRATION_B1_PROBE() {
    int initialValue, finalValue;

    Serial.println("\n==========================\nBOILER 1 PROBE CALIBRATION\n==========================");
    CALIBRATING_B1 = true; // Turn off boiler 1 element
    CALIBRATING_B2 = true; // Turn off boiler 2 element
    CALIBRATION_MODE = true;
    xTaskCreate(CalibrationModeTask, "Calibration Icon Flashing", 4096, NULL, 2, NULL);
    delay(1500);

    // Step 1: Water check to determin if the boiler requires refilling
    Serial.println("Lower the Boiler 1 probe to the lowest position to determine if the probe makes contact with water inside the boiler.");
    delay(1500);
    Serial.println("\nBefore proceeding, ensure you are not touching the probe with your hands or any other tools that would interfere with reading the touch sensitivity of the probe.");
    delay(1500);
    Serial.println("Type 'YES' to proceed.");
    while (true) {
        if (Serial.available()) {
            String input = Serial.readStringUntil('\n');
            input.trim();
            if (input.equalsIgnoreCase("YES")) {
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Short delay
    }
    delay(1500);
    initialValue = B1_TOUCH_VALUE;
    Serial.println("\n==================================================\n");
    Serial.printf("Initial Probe Value: %d\n", initialValue);
    Serial.println("Raise the Boiler 1 probe to the highest position to break contact with the water.");
    delay(1500);
    Serial.println("\nBefore proceeding, ensure you are not touching the probe with your hands or any other tools that would interfere with reading the touch sensitivity of the probe.");
    delay(1500);
    Serial.println("Type 'YES' to proceed.");
    while (true) {
        if (Serial.available()) {
            String input = Serial.readStringUntil('\n');
            input.trim();
            if (input.equalsIgnoreCase("YES")) {
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Short delay
    }
    delay(1500);
    finalValue = B1_TOUCH_VALUE;
    Serial.println("\n==================================================\n");
    Serial.printf("Final Probe Value: %d\n", finalValue);
    if (abs(finalValue - initialValue) > 3) {
        Serial.println("Probe detected water. Proceeding to next step.");
    } else {
        // New step 2:
        delay(1500);
        Serial.println("Probe value did not change. To start refilling Boiler 1 Type 'YES'\nOr type 'NO' to skip Refilling, and proceed to the next step");
        while (true) {
            if (Serial.available()) {
                String input = Serial.readStringUntil('\n');
                input.trim();
                if (input.equalsIgnoreCase("NO")) {
                    break;
                } else if (input.equalsIgnoreCase("YES")) {
                    Serial.println("\n==================================================\n");
                    Serial.println("Set your probe at your desired water fill height");
                    delay(1500);
                    Serial.println("Type 'YES' to proceed.");
                    while (true) {
                        if (Serial.available()) {
                            String input = Serial.readStringUntil('\n');
                            input.trim();
                            if (input.equalsIgnoreCase("YES")) {
                                break;
                            }
                        }
                        vTaskDelay(pdMS_TO_TICKS(10)); // Short delay
                    }
                    Serial.println("\n==================================================\n");
                    Serial.println("System is ready to start refilling Boiler 1. Relays will automatically stop when the probe detects water.\nOr Press 'Enter' to stop refilling at any point");
                    delay(1500);
                    Serial.println("Type 'YES' to start refill.");
                    while (true) {
                        if (Serial.available()) {
                            String input = Serial.readStringUntil('\n');
                            input.trim();
                            if (input.equalsIgnoreCase("YES")) {
                                break;
                            }
                        }
                        vTaskDelay(pdMS_TO_TICKS(10)); // Short delay
                    }
                    Serial.println("\n==================================================\n");
                    Serial.println("\nRefilling Boiler 1...");
                    initialValue = B1_TOUCH_VALUE;
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
                    while (true) {
                        finalValue = B1_TOUCH_VALUE;
                        if (abs(finalValue - initialValue) > 3) {
                            break;
                        }
                        if (Serial.available()) {
                            String stopInput = Serial.readStringUntil('\n');
                            stopInput.trim();
                            if (stopInput.equals("")) { // Detect "Enter" key (newline character)
                                break;
                            }
                        }
                        vTaskDelay(pdMS_TO_TICKS(10)); // Short delay
                    }
                    Serial.println("\n==================================================\n");
                    Serial.println("Boiler probe detected water or refill stopped by user. Stopping refill...");
                    #ifdef PUMP_ON_B1_REFILL
                        digitalWrite(PUMP_PIN, HIGH);  // Water pump OFF
                    #endif

                    #ifdef RELAY1_ON_B1_REFILL
                        digitalWrite(RLY1_PIN, HIGH);  // Relay1 OFF
                    #endif

                    #ifdef RELAY2_ON_B1_REFILL
                        digitalWrite(RLY2_PIN, HIGH);  // Relay2 OFF
                    #endif

                    #ifdef RELAY3_ON_B1_REFILL
                        digitalWrite(RLY3_PIN, HIGH);  // Relay3 OFF
                    #endif

                    #ifdef RELAY4_ON_B1_REFILL
                        digitalWrite(RLY4_PIN, HIGH);  // Relay4 OFF
                    #endif
                    break;
                }
            }
            vTaskDelay(pdMS_TO_TICKS(10)); // Short delay
        }
    }

    delay(1500);
    Serial.println("\n==================================================\n");
    // Step 3: Prompt user to ensure the probe is touching water
    Serial.println("For this next stage, Boiler 1 probe should be contacting water inside the boiler.\nBefore proceeding, ensure you are not touching the probe with your hands or any other tools that would interfere with reading the touch sensitivity of the probe.");
    delay(1500);
    Serial.println("Type 'YES' to proceed.");
    while (true) {
        if (Serial.available()) {
            String input = Serial.readStringUntil('\n');
            input.trim();
            if (input.equalsIgnoreCase("YES")) {
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Short delay
    }
  
    // Step 4: Temporarily store current B1_TOUCH_VALUE
    delay(1500);
    initialValue = B1_TOUCH_VALUE;
    Serial.printf("\n==================================================\nB1 Probe value while contacting water: %d\n", initialValue);
    delay(1500);

    // Step 5: Prompt user to raise the probe above water line
    Serial.println("Raise the Boiler 1 probe above the water line.\nBefore proceeding, ensure you are not touching the probe with your hands or any other tools that would interfere with reading the touch sensitivity of the probe.");
    delay(1500);
    Serial.println("Type 'YES' to proceed.");
    while (true) {
        if (Serial.available()) {
            String input = Serial.readStringUntil('\n');
            input.trim();
            if (input.equalsIgnoreCase("YES")) {
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Short delay
    }

    // Step 6: Verify that B1_TOUCH_VALUE has changed
    delay(1500);
    finalValue = B1_TOUCH_VALUE;
    Serial.printf("\n==================================================\nB1 Probe value while NOT contacting water: %d\n", finalValue);
    delay(1500);
    if (abs(finalValue - initialValue) <= 3) {
        Serial.println("B1 Probe value has not changed.\nPlease ensure that your hardware is correctly setup and the probe is touching OR not touching water inside the boiler when prompted.");
        Serial.println("Rebooting in 5 seconds...");
        delay(5000);
        uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
        delay(20000); // Delay to allow time for reboot
    }

    // Step 7: Calculate the new B1_PROBE value
    int difference = finalValue - initialValue; // Calculate the difference
    double newProbeValue = initialValue + ceil(difference / 2.0); // Add half the difference to the initial value and round up

    // Step 8: Save the new B1_REFILL_TRIG value
    configManager.config.B1_REFILL_TRIG = newProbeValue;
    configManager.config.B1_WTR_PROBE = true;

    Serial.printf("\n==================================================\nBoiler 1 Probe successfully calibrated. B1_REFILL_TRIG value: %.2f\n", newProbeValue);
    Serial.println("Next you'll need to verify that the probe is detecting when a refill is required before these settings will be saved.");
    // Step 9: Verify calibration before saving
    delay(1500);
    Serial.println("\n==================================================\nLower the Boiler 1 probe to make contact with the water.");
    delay(1500);
    Serial.println("Type 'YES' to proceed.");
    while (true) {
        if (Serial.available()) {
            String input = Serial.readStringUntil('\n');
            input.trim();
            if (input.equalsIgnoreCase("YES")) {
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Short delay
    }

    delay(1500); // Delay for 1500ms

    // Step 10: Verify if probe is touching water
    if (B1_TOUCH_VALUE > configManager.config.B1_REFILL_TRIG) {
        Serial.println("\n==================================================\nProbe is NOT contacting water inside the boiler OR the calibration process has not been successful.\nPlease check the probe is contacting water when prompted and try again.");
        Serial.println("Rebooting in 5 seconds...");
        delay(5000);
        uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
        delay(20000); // Delay to allow time for reboot
    }

    Serial.println("\n==================================================\nRaise the Boiler 1 probe above the water.");
    delay(1500);
    Serial.println("Type 'YES' to proceed.");
    while (true) {
        if (Serial.available()) {
            String input = Serial.readStringUntil('\n');
            input.trim();
            if (input.equalsIgnoreCase("YES")) {
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Short delay
    }
    
    delay(1500);
    // Step 11: Verify if probe is not touching water
    if (B1_TOUCH_VALUE < configManager.config.B1_REFILL_TRIG) {
        Serial.println("\n==================================================\nProbe is contacting water inside the boiler OR the calibration process has not been successful.\nPlease check the probe is NOT contacting water when prompted and try again.");
        Serial.println("Rebooting in 5 seconds...");
        delay(5000);
        uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
        delay(20000); // Delay to allow time for reboot
    }

    Serial.println("\n==================================================\nBoiler probe 1 successfully calibrated and settings saved.");
    // Step 12: Save settings and reboot
    configManager.saveSettings();
    delay(1500);
    Serial.println("Rebooting in 5 seconds.");
    delay(5000);
    uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
}


//====================================================
//========== CALIBRATION FOR BOILER 2 PROBE ==========
//====================================================
void CALIBRATION_B2_PROBE() {
    int initialValue, finalValue;
    
    Serial.println("\n==========================\nBOILER 2 PROBE CALIBRATION\n==========================");
    CALIBRATING_B1 = true; // Turn off boiler 1 element
    CALIBRATING_B2 = true; // Turn off boiler 2 element
    CALIBRATION_MODE = true;
    delay(1500);
    // Step 1: Water check to determin if the boiler requires refilling
    Serial.println("Lower the Boiler 2 probe to the lowest position to determine if the probe makes contact with water inside the boiler.");
    delay(1500);
    Serial.println("\nBefore proceeding, ensure you are not touching the probe with your hands or any other tools that would interfere with reading the touch sensitivity of the probe.");
    delay(1500);
    Serial.println("Type 'YES' to proceed.");
    while (true) {
        if (Serial.available()) {
            String input = Serial.readStringUntil('\n');
            input.trim();
            if (input.equalsIgnoreCase("YES")) {
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Short delay
    }
    delay(1500);
    initialValue = B2_TOUCH_VALUE;
    Serial.println("\n==================================================\n");
    Serial.printf("Initial Probe Value: %d\n", initialValue);
    Serial.println("Raise the Boiler 2 probe to the highest position to break contact with the water.");
    delay(1500);
    Serial.println("\nBefore proceeding, ensure you are not touching the probe with your hands or any other tools that would interfere with reading the touch sensitivity of the probe.");
    delay(1500);
    Serial.println("Type 'YES' to proceed.");
    while (true) {
        if (Serial.available()) {
            String input = Serial.readStringUntil('\n');
            input.trim();
            if (input.equalsIgnoreCase("YES")) {
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Short delay
    }

    delay(1500);
    finalValue = B2_TOUCH_VALUE;
    Serial.println("\n==================================================\n");
    Serial.printf("Final Probe Value: %d\n", finalValue);
    if (abs(finalValue - initialValue) > 3) {
        Serial.println("Probe detected water. Proceeding to next step.");
    } else {
        delay(1500);
        Serial.println("Probe value did not change. To start refilling Boiler 2 Type 'YES'\nOr type 'NO' to skip Refilling, and proceed to the next step");
        while (true) {
            if (Serial.available()) {
                String input = Serial.readStringUntil('\n');
                input.trim();
                if (input.equalsIgnoreCase("NO")) {
                    break;
                } else if (input.equalsIgnoreCase("YES")) {
                    Serial.println("\n==================================================\n");
                    Serial.println("Set your probe at your desired water fill height");
                    delay(1500);
                    Serial.println("Type 'YES' to proceed.");
                    while (true) {
                        if (Serial.available()) {
                            String input = Serial.readStringUntil('\n');
                            input.trim();
                            if (input.equalsIgnoreCase("YES")) {
                                break;
                            }
                        }
                        vTaskDelay(pdMS_TO_TICKS(10)); // Short delay
                    }
                    Serial.println("\n==================================================\n");
                    Serial.println("System is ready to start refilling Boiler 2. Relays will automatically stop when the probe detects water.\nOr Press 'Enter' to stop refilling at any point");
                    delay(1500);
                    Serial.println("Type 'YES' to start refill.");
                    while (true) {
                        if (Serial.available()) {
                            String input = Serial.readStringUntil('\n');
                            input.trim();
                            if (input.equalsIgnoreCase("YES")) {
                                break;
                            }
                        }
                        vTaskDelay(pdMS_TO_TICKS(10)); // Short delay
                    }
                    Serial.println("\n==================================================\n");
                    Serial.println("\nRefilling Boiler 2...");
                    initialValue = B2_TOUCH_VALUE;
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
                    while (true) {
                        finalValue = B2_TOUCH_VALUE;
                        if (abs(finalValue - initialValue) > 3) {
                            break;
                        }
                        if (Serial.available()) {
                            String stopInput = Serial.readStringUntil('\n');
                            stopInput.trim();
                            if (stopInput.equals("")) { // Detect "Enter" key (newline character)
                                break;
                            }
                        }
                        vTaskDelay(pdMS_TO_TICKS(10)); // Short delay
                    }
                    Serial.println("\n==================================================\n");
                    Serial.println("Boiler probe detected water or refill stopped by user. Stopping refill...");
                    #ifdef PUMP_ON_B2_REFILL
                        digitalWrite(PUMP_PIN, HIGH);  // Water pump OFF
                    #endif

                    #ifdef RELAY1_ON_B2_REFILL
                        digitalWrite(RLY1_PIN, HIGH);  // Relay1 OFF
                    #endif

                    #ifdef RELAY2_ON_B2_REFILL
                        digitalWrite(RLY2_PIN, HIGH);  // Relay2 OFF
                    #endif

                    #ifdef RELAY3_ON_B2_REFILL
                        digitalWrite(RLY3_PIN, HIGH);  // Relay3 OFF
                    #endif

                    #ifdef RELAY4_ON_B2_REFILL
                        digitalWrite(RLY4_PIN, HIGH);  // Relay4 OFF
                    #endif
                    break;
                }
            }
            vTaskDelay(pdMS_TO_TICKS(10)); // Short delay
        }
    }

    delay(1500);
    Serial.println("\n==================================================\n");

    // Step 3: Prompt user to ensure the probe is touching water
    Serial.println("For this next stage, Boiler 2 probe should be contacting water inside the boiler.\nBefore proceeding, ensure you are not touching the probe with your hands or any other tools that would interfere with reading the touch sensitivity of the probe.");
    delay(1500);
    Serial.println("Type 'YES' to proceed.");
    while (true) {
        if (Serial.available()) {
            String input = Serial.readStringUntil('\n');
            input.trim();
            if (input.equalsIgnoreCase("YES")) {
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Short delay
    }
    
    // Step 4: Temporarily store current B2_TOUCH_VALUE
    delay(1500);
    initialValue = B2_TOUCH_VALUE;
    Serial.printf("\n==================================================\nB2 Probe value while contacting water: %d\n", initialValue);
    delay(1500);
    
    // Step 5: Prompt user to raise the probe above water line
    Serial.println("Raise the Boiler 2 probe above the water line.\nBefore proceeding, ensure you are not touching the probe with your hands or any other tools that would interfere with reading the touch sensitivity value of the probe.");
    delay(1500);
    Serial.println("Type 'YES' to proceed.");
    while (true) {
        if (Serial.available()) {
            String input = Serial.readStringUntil('\n');
            input.trim();
            if (input.equalsIgnoreCase("YES")) {
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Short delay
    }
    
    // Step 6: Verify that B2_TOUCH_VALUE has changed
    delay(1500);
    finalValue = B2_TOUCH_VALUE;
    Serial.printf("\n==================================================\nB2 Probe value while NOT contacting water: %d\n", finalValue);
    delay(1500);
    if (abs(finalValue - initialValue) <= 3) {
        Serial.println("Boiler 2 Probe value has not changed.\nPlease ensure that your hardware is correctly setup and the probe is touching OR not touching water inside the boiler when prompted.");
        Serial.println("Rebooting in 5 seconds...");
        delay(5000);
        uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n")); // Reboots the esp32
        delay(20000); // Delay to allow time for reboot
    }
    
    // Step 7: Calculate and split the difference
    int difference = finalValue - initialValue; // Calculate the difference
    double newProbeValue = initialValue + ceil(difference / 2.0); // Add half the difference to the initial value and round up
    
    // Step 8: Tempararily Save the new B2_REFILL_TRIG value
    configManager.config.B2_REFILL_TRIG = newProbeValue;
    configManager.config.B2_WTR_PROBE = true;

    Serial.printf("\n==================================================\nBoiler 2 Probe successfully calibrated. B2_REFILL_TRIG value: %.2f\n", newProbeValue);
    Serial.println("Next you'll need to verify that the probe is detecting when a refill is required before these settings will be saved.");
    // Step 9: Verify calibration before saving
    delay(1500);
    Serial.println("\n==================================================\nLower the Boiler 2 probe to make contact with the water.");
    delay(1500);
    Serial.println("Type 'YES' to proceed.");
    while (true) {
        if (Serial.available()) {
            String input = Serial.readStringUntil('\n');
            input.trim();
            if (input.equalsIgnoreCase("YES")) {
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Short delay
    }

    delay(1500); // Delay for 1500ms

    // Step 10: Verify if probe is touching water
    if (B2_TOUCH_VALUE > configManager.config.B2_REFILL_TRIG) {
        Serial.println("\n==================================================\nProbe is NOT contacting water inside the boiler OR the calibration process has not been successful.\nPlease check the probe is contacting water when prompted and try again.");
        Serial.println("Rebooting in 5 seconds...");
        delay(5000);
        uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
        delay(20000); // Delay to allow time for reboot
    }

    Serial.println("\n==================================================\nRaise the Boiler 2 probe above the water.");
    delay(1500);
    Serial.println("Type 'YES' to proceed.");
    while (true) {
        if (Serial.available()) {
            String input = Serial.readStringUntil('\n');
            input.trim();
            if (input.equalsIgnoreCase("YES")) {
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Short delay
    }
    
    delay(1500);
    // Step 11: Verify if probe is not touching water
    if (B2_TOUCH_VALUE < configManager.config.B2_REFILL_TRIG) {
        Serial.println("\n==================================================\nProbe is contacting water inside the boiler OR the calibration process has not been successful.\nPlease check the probe is NOT contacting water when prompted and try again.");
        Serial.println("Rebooting in 5 seconds...");
        delay(5000);
        uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
        delay(20000); // Delay to allow time for reboot
    }

    Serial.println("\n==================================================\nBoiler probe 2 successfully calibrated and settings saved.");
    // Step 12:Save settings and reboot
    configManager.saveSettings();
    delay(1500);
    Serial.println("Rebooting in 5seconds.");
    delay(5000);
    uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
}

//=================================================
//========== CALIBRATION RESERVOIR PROBE ==========
//=================================================
void CALIBRATION_RES_PROBE() {
    int initialValue, finalValue;
    
    Serial.println("\n==========================\nRESERVOIR PROBE CALIBRATION\n==========================");
    CALIBRATING_B1 = true; // Turn off boiler 1 element
    CALIBRATING_B2 = true; // Turn off boiler 2 element
    CALIBRATION_MODE = true;
    delay(1500);
    // Step 1: Prompt user to fill the reservoir
    Serial.println("Ensure the Reservoir probe is touching water inside the reservoir tank.\nBefore proceeding, ensure you are not touching the probe with your hands or any other tools that would interfere with reading the touch sensitivity value of the probe.");
    delay(1500);
    Serial.println("Type 'YES' to proceed.");
    while (true) {
        if (Serial.available()) {
            String input = Serial.readStringUntil('\n');
            input.trim();
            if (input.equalsIgnoreCase("YES")) {
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Short delay
    }


    // Request reservoir probe touch value from the slave ESP32
    uart_write_bytes(UART_NUM_1, "GET_RES_PROBE\n", strlen("GET_RES_PROBE\n"));
    delay(1500);

    // Step 2: Temporarily store current RES_TOUCH value
    initialValue = RES_TOUCH_VALUE;
    Serial.printf("\n==================================================\nReservoir Probe value while contacting water: %d\n", initialValue);
    delay(1500);
    
    // Step 3: Prompt user to raise the probe above water line
    Serial.println("Empty the reservoir so the reservoir probe is not contacting water.\nBefore proceeding, ensure you are not touching the probe with your hands or any other tools that would interfere with reading the touch sensitivity value of the probe.");
    delay(1500);
    Serial.println("Type 'YES' to proceed.");
    while (true) {
        if (Serial.available()) {
            String input = Serial.readStringUntil('\n');
            input.trim();
            if (input.equalsIgnoreCase("YES")) {
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Short delay
    }
    // Request reservoir probe touch value from the slave ESP32
    uart_write_bytes(UART_NUM_1, "GET_RES_PROBE\n", strlen("GET_RES_PROBE\n"));
    delay(1500);
    
    // Step 4: Verify that RES_TOUCH value has changed
    finalValue = RES_TOUCH_VALUE;
    Serial.printf("\n==================================================\nReservoir Probe value while NOT contacting water: %d\n", finalValue);
    delay(1500);
    if (abs(finalValue - initialValue) <= 3) {
        Serial.println("Reservoir Probe value has not changed.\nPlease ensure that your hardware is correctly setup and the probe is touching OR not touching water inside the boiler when prompted.");
        Serial.println("Rebooting in 5 seconds...");
        delay(5000);
        uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n")); // Reboots the esp32
        delay(20000); // Delay to allow time for reboot
    }
    
    // Step 5: Calculate and split the difference
    int difference = finalValue - initialValue; // Calculate the difference
    double newProbeValue = initialValue + ceil(difference / 2.0); // Add half the difference to the initial value and round up
    
    // Step 6: Tempararily Save the new RES_PROBE value
    configManager.config.RES_PROBE = newProbeValue;

    Serial.printf("\n==================================================\nReservoir Probe successfully calibrated. RES_PROBE value: %.2f\n", newProbeValue);
    Serial.println("Next you'll need to verify that the probe is detecting when a refill is required before these settings will be saved.");
    // Step 7: Verify calibration before saving
    delay(1500);
    Serial.println("\n==================================================\nRefill the reservoir tank so the probe makes contact with the water.");
    delay(1500);
    Serial.println("Type 'YES' to proceed.");
    while (true) {
        if (Serial.available()) {
            String input = Serial.readStringUntil('\n');
            input.trim();
            if (input.equalsIgnoreCase("YES")) {
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Short delay
    }

    delay(1500); // Delay for 1500ms

    // Request reservoir probe touch value from the slave ESP32
    uart_write_bytes(UART_NUM_1, "GET_RES_PROBE\n", strlen("GET_RES_PROBE\n"));
    delay(1000);

    // Verify if probe is touching water
    if (RES_TOUCH_VALUE > configManager.config.RES_PROBE) {
        Serial.println("\n==================================================\nProbe is NOT contacting water inside the tank OR the calibration process has not been successful.\nPlease check the probe is contacting water when prompted and try again.");
        Serial.println("Rebooting in 5 seconds...");
        delay(5000);
        uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
        delay(20000); // Delay to allow time for reboot
    }

    Serial.println("\n==================================================\nEmpty the reservoir tank so the probe is not touching water.");
    delay(1500);
    Serial.println("Type 'YES' to proceed.");
    while (true) {
        if (Serial.available()) {
            String input = Serial.readStringUntil('\n');
            input.trim();
            if (input.equalsIgnoreCase("YES")) {
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Short delay
    }
    
    // Request reservoir probe touch value from the slave ESP32
    uart_write_bytes(UART_NUM_1, "GET_RES_PROBE\n", strlen("GET_RES_PROBE\n"));
    
    delay(1500);
    // Verify if probe is not touching water
    if (RES_TOUCH_VALUE < configManager.config.RES_PROBE) {
        Serial.println("\n==================================================\nProbe is contacting water inside the reservoir tank OR the calibration process has not been successful.\nPlease check the probe is NOT contacting water when prompted and try again.");
        Serial.println("Rebooting in 5 seconds...");
        delay(5000);
        uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
        delay(20000); // Delay to allow time for reboot
    }

    Serial.println("\n==================================================\nReservoir probe successfully calibrated and settings saved.");
    // Save settings and reboot
    configManager.saveSettings();
    delay(1500);
    Serial.println("Rebooting in 5seconds.");
    delay(5000);
    uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
}

void CALIBRATION_LOAD_CELL() {
    Serial.println("\n==========================\nLOAD CELL CALIBRATION\n==========================");
    CALIBRATION_MODE = true;
    delay(1500);
    Serial.println("Remove all weight from the loadcell.");
    delay(1500);
    Serial.println("Type 'YES' to proceed.");
    while (true) {
        if (Serial.available()) {
            String input = Serial.readStringUntil('\n');
            input.trim();
            if (input.equalsIgnoreCase("YES")) {
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Short delay
    }

    scale.tare(20);
    Serial.print("OFFSET: ");
    Serial.println(scale.get_offset());
    delay(1500);
    Serial.printf("\n==================================================\nPlace a known weight on the loadcell than is atleast 200grams in weight.");
    
    float weight = 0;
    while (true) {
        Serial.println("\nEnter the weight in grams and press 'enter'.");
        while (!Serial.available()) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        String input = Serial.readStringUntil('\n');
        input.trim();
        weight = input.toFloat();
        if (weight > 199) {
            break;
        } else {
            Serial.println("Invalid input. Please enter a weight of atleast 200grams.");
        }
    }

    delay(1500);
    scale.calibrate_scale(weight, 20);
    float newFactor = scale.get_scale();
    Serial.println("\n==================================================\nLoad Cell successfully calibrated.");
    delay(1500);
    Serial.println("New Scale Factor value: ");
    Serial.print(newFactor, 6);

    // Update the SCALE_FACTOR in the configuration directly
    configManager.config.SCALE_FACTOR = newFactor;

    // Save the updated settings
    configManager.saveSettings();
    delay(1500);
    Serial.println("\nRebooting in 5seconds.");
    delay(5000);
    uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
}

//==================================================
//========== CALIBRATION FOR TRANSDUCER 1 ==========
//==================================================

void CALIBRATION_PT1() {
    Serial.println("\n================================\nPRESSURE TRANSDUCER 1 CALIBRATION\n================================");
    CALIBRATING_B1 = true; // Turn off boiler 1 element
    CALIBRATING_B2 = true; // Turn off boiler 2 element
    CALIBRATION_MODE = true;
    uart_write_bytes(UART_NUM_1, "CALIBRATING_PT2\n", strlen("CALIBRATING_PT2\n")); // turn off boiler 2 element
    Serial.println("During calibration, the boiler elements will be turned ON and OFF as required.");
    delay(1500);

    // prompt the user to set a B1_KPA target pressure
    Serial.println("Please enter your desired target pressure for Boiler 1 in kPa (acceptable range: 10 ~ 200 kPa):");
    float b1_kpa_target = 0;
    while (true) {
        while (!Serial.available()) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        String input = Serial.readStringUntil('\n');
        input.trim();
        b1_kpa_target = input.toFloat();
        if (b1_kpa_target >= 10 && b1_kpa_target <= 200) {
            configManager.config.B1_KPA = b1_kpa_target;
            break;
        } else {
            Serial.printf("Target pressure for Boiler 1 (B1_KPA) set to %.2f kPa\n", b1_kpa_target);
        }
    }
    delay(1500);
    Serial.printf("B1_KPA target set to %.2f\n", b1_kpa_target);

    Serial.println("\n==================================================");
    Serial.println("\nEnter the maximum pressure rating (in kPa) of the pressure transducer you are using (acceptable range: 100 ~ 1000 kPa):");

    // Prompt for PT1_KPA
    float pt1_kpa = 0;
    while (true) {
        while (!Serial.available()) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        String input = Serial.readStringUntil('\n');
        input.trim();
        pt1_kpa = input.toFloat();
        if (pt1_kpa >= 100 && pt1_kpa <= 1000) {
            break;
        } else {
            Serial.println("Invalid input. Please enter a value between 100 ~ 1000 kPa.");
        }
    }
    // Save PT1_KPA to LFS
    configManager.config.PT1_KPA = pt1_kpa;
    delay(1500);
    Serial.printf("Maximum pressure rating for PT1 (PT1_KPA) set to %.2f kPa\n", pt1_kpa);

    // Confirm ambient pressure
    Serial.println("\n==================================================");
    Serial.println("\nIf the boiler has built up any pressure, allow it to cool down until there is NO pressure inside.");
    Serial.println("Ensure the boiler has 0 pressure, then type 'YES' to proceed when ready.");
    while (true) {
        while (!Serial.available()) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        String input = Serial.readStringUntil('\n');
        input.trim();
        if (input.equalsIgnoreCase("YES")) {
            break;
        }
    }
    // Read current voltage
    float pt1_min_v = analogRead(PRESSURE_SENSOR_PIN) * (3.3 / 4095.0) * VOLTAGE_DIVIDER_RATIO;
    // Save PT1_MIN_V to LFS
    configManager.config.PT1_MIN_V = pt1_min_v;
    delay(1500);
    Serial.printf("PT1_MIN_V set to %.2f\n", pt1_min_v);

    // Prompt for theoretical max voltage
    Serial.println("\n==================================================");
    Serial.println("\nEnter the maximum signal voltage from the transducer: (acceptable range: 3 ~ 5):");
    float theoretical_max_voltage = 0;
    while (true) {
        while (!Serial.available()) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        String input = Serial.readStringUntil('\n');
        input.trim();
        theoretical_max_voltage = input.toFloat();
        if (theoretical_max_voltage >= 3 && theoretical_max_voltage <= 5.1) {
            break;
        } else {
            Serial.println("Invalid input. Maximum acceptable voltage range is 3 to 5V.");
        }
    }
    // Calculate initial PT1_MAX_V
    float pt1_max_v = theoretical_max_voltage;
    delay(1500);
    Serial.printf("Initial PT1_MAX_V calculated as %.2f\n", pt1_max_v);

    CALIBRATING_B1 = false; // Enable the boiler to start to heat up

    // Wait for pressure build-up
    Serial.println("\n==================================================");
    Serial.println("\nDO NOT LEAVE THE MACHINE UNATTENDED!!!\nBoiler 1 is now turned ON. Allow the boiler to build up pressure, which may take several minutes.");
    Serial.println("Closely monitor the boiler pressure and turn off immediately if you suspect the pressure is above safe operating limits.");
    Serial.printf("When the pressure for Boiler 1 (shown on the display) is around the target pressure of: %.2f kPa, type 'YES' to proceed.\n", b1_kpa_target);
    while (true) {
        while (!Serial.available()) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        String input = Serial.readStringUntil('\n');
        input.trim();
        if (input.equalsIgnoreCase("YES")) {
            break;
        }
    }
    // Prompt for actual boiler pressure
    delay(1500);
    Serial.println("\n==================================================");
    Serial.println("\nDO NOT use the pressure shown on the display for this next step!");
    Serial.println("\nUsing a separate and reliable pressure gauge connected to Boiler 1, enter the current boiler pressure in kPa:");
    float actual_pressure = 0;
    while (true) {
        while (!Serial.available()) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        String input = Serial.readStringUntil('\n');
        input.trim();
        actual_pressure = input.toFloat();
        if (actual_pressure >= 20 && actual_pressure <= 200) {
            break;
        } else {
            Serial.println("Invalid input. Please enter a valid pressure reading between 20 ~ 200 kPa.");
        }
    }
    // Read current voltage at pressure
    float voltage_at_pressure = analogRead(PRESSURE_SENSOR_PIN) * (3.3 / 4095.0) * VOLTAGE_DIVIDER_RATIO; // Adjust for voltage divider

    // Calculate corrected PT1_MAX_V
    pt1_max_v = pt1_min_v + ((voltage_at_pressure - pt1_min_v) * (pt1_kpa / actual_pressure));
    // Save PT1_MAX_V to LFS
    configManager.config.PT1_MAX_V = pt1_max_v;
    delay(1500);
    Serial.printf("Maximum voltage for PT1 (PT1_MAX_V) set to %.2f V\n", pt1_max_v);
    
    // prompt the user to wait for the pressure to stabilize. Once pressure is stable, the user should enter the pressure (in kPa) reading from their pressure gauge (acceptable range is 20 ~ 120)
    Serial.println("\n==================================================");
    Serial.println("\nPressure Transducer calibrated!");
    Serial.println("Before saving the settings, you need to verify the pressure reading on the display closely matches the pressure reading from your separate pressure gauge.");
    Serial.printf("Allow a couple of minutes for the boiler to heat/cool to the target pressure: %.2f kPa\n", b1_kpa_target);
    Serial.println("For this next step, the pressure on the display will be compared to the pressure gauge. If the two readings are within a 5% margin of error, the calibration settings will be saved.");
    Serial.println("To proceed, enter the pressure (in kPa) from the pressure gauge (acceptable range: 10 ~ 200 kPa):");

    float stable_pressure = 0;
    while (true) {
        while (!Serial.available()) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        String input = Serial.readStringUntil('\n');
        input.trim();
        stable_pressure = input.toFloat();
        if (stable_pressure >= 10 && stable_pressure <= 200) {
            break;
        } else {
            Serial.println("Invalid input. Please enter a value between 10 ~ 200 kPa.");
        }
    }

    // Compare the user submitted pressure with new calibrated PRESSURE_KPA_1
    float difference = abs(stable_pressure - PRESSURE_KPA_1) / PRESSURE_KPA_1;
    if (difference < 0.05) { // Difference is less than 5%
        configManager.saveSettings();
        Serial.println("\n==================================================");
        Serial.println("\nCalibration for Pressure Transducer 1 successful!");
    } else {
        Serial.println("\n==================================================");
        Serial.println("\nCalibration for Pressure Transducer 1 FAILED!\nThe difference between the transducer pressure reading and the user-submitted pressure reading is more than 5%.");
        delay(1500);
        Serial.println("\nRebooting in 5 seconds.");
    }

    delay(1500);
    Serial.println("\nRebooting in 5 seconds.");
    delay(5000);
    uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
}


//==================================================
//========== CALIBRATION FOR TRANSDUCER 2 ==========
//==================================================

void CALIBRATION_PT2() {
    Serial.println("\n=============================\nPRESSURE TRANSDUCER 2 CALIBRATION\n=============================");
    CALIBRATING_B1 = true; // Turn off boiler 1 element
    CALIBRATION_MODE = true;
    uart_write_bytes(UART_NUM_1, "CALIBRATING_B2\n", strlen("CALIBRATING_B2\n")); // turn off the boiler element for the inital part of calibration
    Serial.println("During calibration, the boiler elements will be turned ON and OFF as required.");
    delay(1500);

    // prompt the user to set a B2_KPA target pressure
    Serial.println("Please enter a target pressure for Boiler 2 in kPa (acceptable range: 10 ~ 200 kPa):");
    float b2_kpa_target = 0;
    while (true) {
        while (!Serial.available()) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        String input = Serial.readStringUntil('\n');
        input.trim();
        b2_kpa_target = input.toFloat();
        if (b2_kpa_target >= 10 && b2_kpa_target <= 200) {
            configManager.config.B2_KPA = b2_kpa_target;
            delay(250);
            SEND_PT2_SETTINGS = true;
            break;
        } else {
            Serial.printf("Target pressure for Boiler 2 (B2_KPA) set to %.2f kPa\n", b2_kpa_target);
        }
    }
    delay(1500);
    Serial.printf("B2_KPA target set to %.2f\n", b2_kpa_target);

    Serial.println("\n==================================================");
    Serial.println("\nEnter the maximum pressure rating (in kPa) of the pressure transducer you are using (acceptable range: 100 ~ 1000 kPa):");


    // Prompt for PT2_KPA
    float pt2_kpa = 0;
    while (true) {
        while (!Serial.available()) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        String input = Serial.readStringUntil('\n');
        input.trim();
        pt2_kpa = input.toFloat();
        if (pt2_kpa >= 100 && pt2_kpa <= 1000) {
            break;
        } else {
            Serial.println("Invalid input. Please enter a value between 100 ~ 1000.");
        }
    }
    // Save PT2_KPA to LFS
    configManager.config.PT2_KPA = pt2_kpa;
    delay(250);
    SEND_PT2_SETTINGS = true;
    delay(1500);
    Serial.printf("Maximum pressure rating for PT2 (PT2_KPA) set to %.2f kPa\n", pt2_kpa);

    // Confirm ambient pressure
    Serial.println("\n==================================================");
    Serial.println("\nIf the boiler has built up any pressure, allow it to cool down until there is NO pressure inside.");
    Serial.println("Ensure the boiler has 0 pressure, then type 'YES' to proceed when ready.");
    while (true) {
        while (!Serial.available()) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        String input = Serial.readStringUntil('\n');
        input.trim();
        if (input.equalsIgnoreCase("YES")) {
            break;
        }
    }

    // Request voltage from slave ESP32
    uart_write_bytes(UART_NUM_1, "GET_PT2_VOLTAGE\n", strlen("GET_PT2_VOLTAGE\n"));
    delay(1000);
    // Read PT2_VOLTAGE from the global variable
    float pt2_min_v = PT2_VOLTAGE * VOLTAGE_DIVIDER_RATIO;

    // Save PT2_MIN_V to LFS
    configManager.config.PT2_MIN_V = pt2_min_v;
    delay(250);
    SEND_PT2_SETTINGS = true;
    Serial.printf("\n==================================================\nPT2_MIN_V set to %.2f\n", pt2_min_v);

    // Prompt for theoretical max voltage
    Serial.println("\n\nEnter the maximum signal voltage from the transducer (acceptable range: 3 ~ 5):");
    float theoretical_max_voltage = 0;
    while (true) {
        while (!Serial.available()) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        String input = Serial.readStringUntil('\n');
        input.trim();
        theoretical_max_voltage = input.toFloat();
        if (theoretical_max_voltage >= 3 && theoretical_max_voltage <= 5.1) {
            break;
        } else {
            Serial.println("Invalid input. Maximum acceptable voltage range is 3 to 5V.");
        }
    }
    // Calculate initial PT2_MAX_V
    float pt2_max_v = theoretical_max_voltage;
    delay(1500);
    Serial.printf("Initial PT2_MAX_V calculated as %.2f\n", pt2_max_v);

    uart_write_bytes(UART_NUM_1, "STOP_CALIBRATING_B2\n", strlen("STOP_CALIBRATING_B2\n")); // Enable the boiler to start to heat up

    // Wait for pressure build-up
    Serial.println("\n==================================================");
    Serial.println("\nDO NOT LEAVE THE MACHINE UNATTENDED!!!\nBoiler 2 is now turned ON. Allow the boiler to build up pressure, which may take several minutes.");
    Serial.println("Closely monitor the boiler pressure and turn off immediately if you suspect the pressure is above safe operating limits.");
    Serial.printf("When the pressure for Boiler 2 (shown on the display) is around the target pressure of: %.2f kPa, type 'YES' to proceed.\n", b2_kpa_target);
    while (true) {
        while (!Serial.available()) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        String input = Serial.readStringUntil('\n');
        input.trim();
        if (input.equalsIgnoreCase("YES")) {
            break;
        }
    }
    // Request voltage from slave ESP32
    uart_write_bytes(UART_NUM_1, "GET_PT2_VOLTAGE\n", strlen("GET_PT2_VOLTAGE\n"));
    delay(1000);
    
    // Prompt for actual pressure
    delay(1500);
    Serial.println("\n==================================================");
    Serial.println("\nDO NOT use the pressure shown on the display for this next step!");
    Serial.println("\nUsing a separate and reliable pressure gauge connected to Boiler 1, enter the current boiler pressure in kPa:");
    float actual_pressure = 0;
    while (true) {
        while (!Serial.available()) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        String input = Serial.readStringUntil('\n');
        input.trim();
        actual_pressure = input.toFloat();
        if (actual_pressure > 0 && actual_pressure <= pt2_kpa) {
            break;
        } else {
            Serial.println("Invalid input. Please enter a valid pressure reading.");
        }
    }

    // Read current voltage at pressure
    float voltage_at_pressure = PT2_VOLTAGE * VOLTAGE_DIVIDER_RATIO;

    // Calculate corrected PT2_MAX_V
    pt2_max_v = pt2_min_v + ((voltage_at_pressure - pt2_min_v) * (pt2_kpa / actual_pressure));
    // Save PT2_MAX_V to LFS
    configManager.config.PT2_MAX_V = pt2_max_v;
    delay(250);
    SEND_PT2_SETTINGS = true;
    delay(1500);
    Serial.printf("Maximum voltage for PT2 (PT2_MAX_V) set to %.2f V\n", pt2_max_v);
    delay(1500);

    // prompt the user to wait for the pressure to stabilize. Once pressure is stable, the user should enter the pressure (in kPa) reading from their pressure gauge (acceptable range is 20 ~ 120)
    Serial.println("\n==================================================");
    Serial.println("\nPressure Transducer calibrated!");
    Serial.println("Before saving the settings, you need to verify the pressure reading on the display closely matches the pressure reading from your separate pressure gauge.");
    Serial.printf("Allow a couple of minutes for the boiler to heat/cool to the target pressure: %.2f kPa\n", b2_kpa_target);
    Serial.println("For this next step, the pressure on the display will be compared to the pressure gauge. If the two readings are within a 5% margin of error, the calibration settings will be saved.");
    Serial.println("To proceed, enter the pressure (in kPa) from the pressure gauge (acceptable range: 10 ~ 200 kPa):");

    float stable_pressure = 0;
    while (true) {
        while (!Serial.available()) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        String input = Serial.readStringUntil('\n');
        input.trim();
        stable_pressure = input.toFloat();
        if (stable_pressure >= 10 && stable_pressure <= 200) {
            break;
        } else {
            Serial.println("Invalid input. Please enter a value between 10 ~ 200 kPa.");
        }
    }

    // Compare the user submitted pressure with new calibrated PRESSURE_KPA_1
    float difference = abs(stable_pressure - PRESSURE_KPA_2) / PRESSURE_KPA_2;
    if (difference < 0.05) { // Difference is less than 5%
        configManager.saveSettings();
        Serial.println("\n==================================================");
        Serial.println("\nCalibration for Pressure Transducer 2 successful!");
    } else {
        Serial.println("\n==================================================");
        Serial.println("\nCalibration for Pressure Transducer 2 FAILED!\nThe difference between the transducer pressure reading and the user-submitted pressure reading is more than 5%.");
        delay(1500);
        Serial.println("\nRebooting in 5 seconds.");
    }

    delay(1500);
    Serial.println("\nRebooting in 5 seconds.");
    delay(5000);
    uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
}
