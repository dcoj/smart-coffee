#include "HEADER.h"

// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================

void UART_TASK(void *pvParameters) {
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(HEARTBEAT_SEND_INTERVAL_MS);
    xLastWakeTime = xTaskGetTickCount();
    TickType_t lastHeartbeatReceived = xTaskGetTickCount();
    TickType_t lastDataSentTime = xTaskGetTickCount();

    char message[32]; // Buffer for outgoing messages
    char temp1Message[32]; // Buffer for outgoing temperature message
    char temp2Message[32]; // Buffer for outgoing temperature message
    char KPA2Message[32];// Buffer for outgoing kPa message

    bool configReceived = false; // Flag to track if boot setup config has been received
    bool receivePT2Settings = false;

    if (ENTER_SAFE_MODE) {
        vTaskSuspend(NULL); // Permanently suspend this task
    }



    while (1) {
        // ============================
        // ==== OUTGOING UART DATA ====
        // ============================
        
        if (!configReceived) {
        // Send BOOT_SETUP message on boot-up
        uart_write_bytes(UART_NUM_1, "BOOT_SETUP\n", strlen("BOOT_SETUP\n"));
        }


        // Send heartbeat every 2 seconds
        if (xTaskGetTickCount() - xLastWakeTime >= xFrequency) {
            uart_write_bytes(UART_NUM_1, "HEARTBEAT\n", strlen("HEARTBEAT\n"));
            xLastWakeTime = xTaskGetTickCount();
        }

        // Send ULTRASONIC WATER PERCENTAGE
        if (xMutexWaterResPercentage != NULL) {
            if (xSemaphoreTake(xMutexWaterResPercentage, (TickType_t)10) == pdTRUE) {
                // Format and send the ULTRA_WATER_PERCENTAGE value with a newline
                snprintf(message, sizeof(message), "UWP:%d\n", ULTRA_WATER_PERCENTAGE);
                uart_write_bytes(UART_NUM_1, message, strlen(message));
                xSemaphoreGive(xMutexWaterResPercentage);
            }
        }
        // SEND TEMP 1
        if (configManager.config.TEMP_SENSOR_1) { 
            if (xMutexTemprature1 != NULL) {
                if (xSemaphoreTake(xMutexTemprature1, (TickType_t)10) == pdTRUE) {
                    // Format and send the TEMP_C_1 value with a newline
                    snprintf(temp1Message, sizeof(temp1Message), "TEMP1:%.2f\n", TEMP_C_1);
                    uart_write_bytes(UART_NUM_1, temp1Message, strlen(temp1Message));
                    xSemaphoreGive(xMutexTemprature1);
                }
            }
        }
            // SEND TEMP 2
        if (configManager.config.TEMP_SENSOR_2) {
            if (xMutexTemprature2 != NULL) {
                if (xSemaphoreTake(xMutexTemprature2, (TickType_t)10) == pdTRUE) {
                    // Format and send the TEMP_C_2 value with a newline
                    snprintf(temp2Message, sizeof(temp2Message), "TEMP2:%.2f\n", TEMP_C_2);
                    uart_write_bytes(UART_NUM_1, temp2Message, strlen(temp2Message));
                    xSemaphoreGive(xMutexTemprature2);
                }
            }
        }

        // SEND PRESSURE TRANSDUCER 2 kPa
        if (xMutexTemprature2 != NULL) {
            if (xSemaphoreTake(xMutexPressure, (TickType_t)10) == pdTRUE) {
                // Format and send the PRESSURE_KPA_2 value with a newline
                snprintf(KPA2Message, sizeof(KPA2Message), "KPA2:%.2f\n", PRESSURE_KPA_2);
                uart_write_bytes(UART_NUM_1, KPA2Message, strlen(KPA2Message));
                xSemaphoreGive(xMutexPressure);
            }
        }

        // Send Boiler 1 probe value
        snprintf(message, sizeof(message), "B1_TOUCH:%d\n", BOILER_1_PROBE_TOUCH_VALUE);
        uart_write_bytes(UART_NUM_1, message, strlen(message));

        // Send Boiler 2 probe value
        snprintf(message, sizeof(message), "B2_TOUCH:%d\n", BOILER_2_PROBE_TOUCH_VALUE);
        uart_write_bytes(UART_NUM_1, message, strlen(message));

        // Send Reservoir refill message
        if (RESERVOIR_REFILL_REQUIRED_UART) {
            uart_write_bytes(UART_NUM_1, "REFILL_RESERVOIR\n", strlen("REFILL_RESERVOIR\n"));
            RESERVOIR_REFILL_REQUIRED_UART = false;
        }

        // Send Reservoir refill message
        if (RESERVOIR_FULL_UART) {
            uart_write_bytes(UART_NUM_1, "RESERVOIR_FULL\n", strlen("RESERVOIR_FULL\n"));
            RESERVOIR_FULL_UART = false; // Reset the flag after sending message
        }

        // Send the state of element 2 
        if (digitalRead(ELEMENT_2) == LOW) {
            uart_write_bytes(UART_NUM_1, "E2_ON\n", strlen("E2_ON\n"));
        } else if (digitalRead(ELEMENT_2) == HIGH) {
            uart_write_bytes(UART_NUM_1, "E2_OFF\n", strlen("E2_OFF\n"));
        }

        // SEND SENSOR SERIAL PRINTING DATA TO MASTER ESP32
        const TickType_t xSensorFrequency = pdMS_TO_TICKS(500);
        if (xTaskGetTickCount() - lastDataSentTime >= xSensorFrequency) {
            lastDataSentTime = xTaskGetTickCount();

            // SEND SENSOR SERIAL PRINTING DATA TO MASTER ESP32
            if (configManager.config.SERIAL_PRINTING) {
                uart_write_bytes(UART_NUM_1, "MSPS\n", strlen("MSPS\n")); // Message to tell the master to start serial printing it'd own sensor data
                uart_write_bytes(UART_NUM_1, "SETTINGS_START\n", strlen("SETTINGS_START\n"));
                if (configManager.config.TRANSDUCER_2) {
                    // Pressure sensor data
                    int rawValue = analogRead(PRESSURE_SENSOR_PIN);
                    double voltage = rawValue * (5.0 / 4095.0);
                    snprintf(message, sizeof(message), " | PT2 Volts: %.2f", voltage);
                    uart_write_bytes(UART_NUM_1, message, strlen(message));
                }

                if (configManager.config.WTR_RES_PROBE) {
                    snprintf(message, sizeof(message), " | Res probe: %d", SP_RES_PROBE);
                    uart_write_bytes(UART_NUM_1, message, strlen(message));
                }

                snprintf(message, sizeof(message), " | B1 probe: %d", SP_BOILER_1_PROBE);
                uart_write_bytes(UART_NUM_1, message, strlen(message));

                if (configManager.config.DUAL_BOILER) {
                    snprintf(message, sizeof(message), " | B2 probe: %d", SP_BOILER_2_PROBE);
                    uart_write_bytes(UART_NUM_1, message, strlen(message));
                }
                uart_write_bytes(UART_NUM_1, "\nSETTINGS_END\n", strlen("\nSETTINGS_END\n"));
            }
        }

        // ============================
        // ==== INCOMING UART DATA ====
        // ============================

        int len = uart_read_bytes(UART_NUM_1, activeBuffer, UART_RX_BUF_SIZE - 1, 20 / portTICK_RATE_MS);
        if (len > 0) {
            activeBuffer[len] = '\0'; // Null-terminate the received data
            switchBuffers(); // Switch the active and processing buffers

            // Uncomment to enable serial printing incoming uart data
            //Serial.print("Received: ");
            //Serial.println((char *)processingBuffer); // Use processingBuffer here

            char *token = strtok((char *)processingBuffer, "\n"); //
            while (token != NULL) {
                if (strcmp(token, "HEARTBEAT") == 0) {
                    lastHeartbeatReceived = xTaskGetTickCount();
                    HANDSHAKE = true;

                // Receive the boot setup shared values from the master
                } else if (!configReceived) {
                    StaticJsonDocument<1028> doc;
                    DeserializationError error = deserializeJson(doc, token);
                    if (!error) {
                        configManager.config.B2_REFILL_TRIG = doc["B2_REFILL_TRIG"];
                        configManager.config.SERIAL_PRINTING = doc["SERIAL_PRINTING"];
                        configManager.config.ULTRASONIC_SENSOR = doc["ULTRASONIC_SENSOR"];
                        configManager.config.DUAL_BOILER = doc["DUAL_BOILER"];
                        configManager.config.TEMP_SENSOR_1 = doc["TEMP_SENSOR_1"];
                        configManager.config.TEMP_SENSOR_2 = doc["TEMP_SENSOR_2"];
                        configManager.config.TRANSDUCER_2 = doc["TRANSDUCER_2"];
                        configManager.config.B2_KPA = doc["B2_KPA"];
                        configManager.config.B2_TEMP = doc["B2_TEMP"];
                        configManager.config.B2_KP = doc["B2_KP"];
                        configManager.config.B2_KI = doc["B2_KI"];
                        configManager.config.B2_KD = doc["B2_KD"];
                        configManager.config.RES_PROBE = doc["RES_PROBE"];
                        configManager.config.RES_FULL = doc["RES_FULL"];
                        configManager.config.RES_EMPTY = doc["RES_EMPTY"];
                        configManager.config.RES_LOW = doc["RES_LOW"];
                        configManager.config.B2_PWR = doc["B2_PWR"];
                        configManager.config.B2_MAX_OT = doc["B2_MAX_OT"];
                        configManager.config.B2_MIN_OT = doc["B2_MIN_OT"];
                        configManager.config.B2_PWM = doc["B2_PWM"];
                        configManager.config.B2_STABLE_PWM = doc["B2_STABLE_PWM"];
                        configManager.config.B2_STABLE_TH = doc["B2_STABLE_TH"];
                        configManager.config.PT2_MIN_V = doc["PT2_MIN_V"];
                        configManager.config.PT2_MAX_V = doc["PT2_MAX_V"];
                        configManager.config.PT2_KPA = doc["PT2_KPA"];
                        configManager.config.B2_MIN_KPA = doc["B2_MIN_KPA"];
                        configManager.config.B2_MAX_KPA = doc["B2_MAX_KPA"];
                        configManager.config.TEMP_1_MIN = doc["TEMP_1_MIN"];
                        configManager.config.TEMP_1_MAX = doc["TEMP_1_MAX"];
                        configManager.config.TEMP_2_MIN = doc["TEMP_2_MIN"];
                        configManager.config.TEMP_2_MAX = doc["TEMP_2_MAX"];
                        configManager.config.TRANSDUCER_2_KPA_SAFE = doc["TRANSDUCER_2_KPA_SAFE"];
                        configManager.config.TEMP_1_SAFE = doc["TEMP_1_SAFE"];
                        configManager.config.TEMP_2_SAFE = doc["TEMP_2_SAFE"];
                        configManager.config.TARGET_TEMP_B2 = doc["TARGET_TEMP_B2"];
                        configManager.config.PID_CONTROL_B2 = doc["PID_CONTROL_B2"];
                        configManager.config.RES_SW = doc["RES_SW"];
                        configManager.config.RES_SW_NC = doc["RES_SW_NC"];
                        configManager.config.WTR_RES_PROBE = doc["WTR_RES_PROBE"];
                        configManager.config.US_LVL_DETECT = doc["US_LVL_DETECT"];
                        configManager.config.B1_WTR_PROBE = doc["B1_WTR_PROBE"];
                        configManager.config.B2_WTR_PROBE = doc["B2_WTR_PROBE"];
                        configManager.saveSettings();
                        configReceived = true; // Set flag to true after receiving config
                    }

                } else if (strcmp(token, "PT2_INCOMING") == 0) {
                    // Expecting the next message to be the PT2 settings JSON
                    receivePT2Settings = true; // Set a flag to indicate the next message is PT2 settings
                } else if (receivePT2Settings) {
                    // Handle receiving PT2 settings JSON
                    StaticJsonDocument<512> doc;
                    DeserializationError error = deserializeJson(doc, token);
                    if (!error) {
                        if (doc.containsKey("B2_KPA")) {
                            configManager.config.B2_KPA = doc["B2_KPA"];
                        }
                        if (doc.containsKey("PT2_MAX_V")) {
                            configManager.config.PT2_MAX_V = doc["PT2_MAX_V"];
                        }
                        if (doc.containsKey("PT2_MIN_V")) {
                            configManager.config.PT2_MIN_V = doc["PT2_MIN_V"];
                        }
                        if (doc.containsKey("PT2_KPA")) {
                            configManager.config.PT2_KPA = doc["PT2_KPA"];
                        }
                    }
                    receivePT2Settings = false; // Reset the flag

                } else if (strcmp(token, "PRINT_SETTINGS") == 0) {
                    sendSettings();

                // Enable/Disable Element 2 during calibration
                } else if (strcmp(token, "CALIBRATING_B2") == 0) {
                    CALIBRATING_B2 = true;
                } else if (strcmp(token, "STOP_CALIBRATING_B2") == 0) {
                    CALIBRATING_B2 = false;

                // ENABLE/DISABLE RELAYS
                } else if (strcmp(token, "ENABLE_RELAYS") == 0) {
                    CALIBRATING_B2 = false;
                } else if (strcmp(token, "DISABLE_RELAYS") == 0) {
                    CALIBRATING_B2 = true;

                // SEND PT2 VOLTAGE TO MASTER
                } else if (strcmp(token, "GET_PT2_VOLTAGE") == 0) {
                    // Read the voltage from PT2
                    float pt2_voltage = analogRead(PRESSURE_SENSOR_PIN) * (3.3 / 4095.0);
                    
                    // Create a response message with the voltage value
                    char response[50];
                    snprintf(response, sizeof(response), "PT2_VOLTAGE:%.2f\n", pt2_voltage);
                    
                    // Send the voltage value back to the master
                    uart_write_bytes(UART_NUM_1, response, strlen(response));

                // SEND RESERVOIR PROBE TOUCH VALUE MASTER
                } else if (strcmp(token, "GET_RES_PROBE") == 0) {
                    snprintf(message, sizeof(message), "RES_TOUCH:%d\n", RES_PROBE_TOUCH_VALUE);
                    uart_write_bytes(UART_NUM_1, message, strlen(message));

                // ENTER SAFE MODE
                } else if (strcmp(token, "ENTER_SM") == 0) {
                    ENTER_SAFE_MODE = true; // Set the error flag 

                // ==================================================================
                // ====== PROCESS INCOMING UART DATA TO UPDATE LITTLEFS VALUES ======
                // ==================================================================

                } else if (strncmp(token, "B2_REFILL_TRIG ", 7) == 0) {
                    double B2_REFILL_TRIG_UART;
                    if (sscanf(token, "B2_REFILL_TRIG %lf", &B2_REFILL_TRIG_UART) == 1) {
                        configManager.config.B2_REFILL_TRIG = B2_REFILL_TRIG_UART;
                        configManager.saveSettings();
                        uart_write_bytes(UART_NUM_1, "B2 REFILL TRIG Updated\n", strlen("B2 REFILL TRIG Updated\n")); // Send confirmation back to master
                        delay(2000); // Wait a moment to ensure the message is sent before rebooting
                        esp_restart();
                    }
                } else if (strncmp(token, "B2_KPA ", 7) == 0) {
                    double B2_KPA_UART;
                    if (sscanf(token, "B2_KPA %lf", &B2_KPA_UART) == 1) {
                        configManager.config.B2_KPA = B2_KPA_UART;
                        configManager.saveSettings();
                        uart_write_bytes(UART_NUM_1, "B2 KPA Updated\n", strlen("B2 KPA Updated\n")); // Send confirmation back to master
                        delay(2000); // Wait a moment to ensure the message is sent before rebooting
                        esp_restart();
                    }
                } else if (strncmp(token, "B2_TEMP ", 8) == 0) {
                    double B2_TEMP_UART;
                    if (sscanf(token, "B2_TEMP %lf", &B2_TEMP_UART) == 1) {
                        configManager.config.B2_TEMP = B2_TEMP_UART;
                        configManager.saveSettings();
                        uart_write_bytes(UART_NUM_1, "B2 TEMP Updated\n", strlen("B2 TEMP Updated\n")); // Send confirmation back to master
                        delay(2000); // Wait a moment to ensure the message is sent before rebooting
                        esp_restart();
                    }
                } else if (strncmp(token, "B2_KP ", 6) == 0) {
                    double B2_KP_UART;
                    if (sscanf(token, "B2_KP %lf", &B2_KP_UART) == 1) {
                        configManager.config.B2_KP = B2_KP_UART;
                        configManager.saveSettings();
                        uart_write_bytes(UART_NUM_1, "B2 Kp Updated\n", strlen("B2 Kp Updated\n")); // Send confirmation back to master
                        delay(2000); // Wait a moment to ensure the message is sent before rebooting
                        esp_restart();
                    }
                } else if (strncmp(token, "B2_KI ", 6) == 0) {
                    double B2_KI_UART;
                    if (sscanf(token, "B2_KI %lf", &B2_KI_UART) == 1) {
                        configManager.config.B2_KI = B2_KI_UART;
                        configManager.saveSettings();
                        uart_write_bytes(UART_NUM_1, "B2 Ki Updated\n", strlen("B2 Ki Updated\n")); // Send confirmation back to master
                        delay(2000); // Wait a moment to ensure the message is sent before rebooting
                        esp_restart();
                    }
                } else if (strncmp(token, "B2_KD ", 6) == 0) {
                    double B2_KD_UART;
                    if (sscanf(token, "B2_KD %lf", &B2_KD_UART) == 1) {
                        configManager.config.B2_KD = B2_KD_UART;
                        configManager.saveSettings();
                        uart_write_bytes(UART_NUM_1, "B2 Kd Updated\n", strlen("B2 Kd Updated\n")); // Send confirmation back to master
                        delay(2000); // Wait a moment to ensure the message is sent before rebooting
                        esp_restart();
                    }
                } else if (strncmp(token, "RES_PROBE ", 10) == 0) {
                    double RES_PROBE_UART;
                    if (sscanf(token, "RES_PROBE %lf", &RES_PROBE_UART) == 1) {
                        configManager.config.RES_PROBE = RES_PROBE_UART;
                        configManager.saveSettings();
                        uart_write_bytes(UART_NUM_1, "RES PROBE Updated\n", strlen("RES PROBE Updated\n")); // Send confirmation back to master
                        delay(2000); // Wait a moment to ensure the message is sent before rebooting
                        esp_restart();
                    }
                } else if (strncmp(token, "RES_FULL ", 9) == 0) {
                    double RES_FULL_UART;
                    if (sscanf(token, "RES_FULL %lf", &RES_FULL_UART) == 1) {
                        configManager.config.RES_FULL = RES_FULL_UART;
                        configManager.saveSettings();
                        uart_write_bytes(UART_NUM_1, "RES FULL Updated\n", strlen("RES FULL Updated\n")); // Send confirmation back to master
                        delay(2000); // Wait a moment to ensure the message is sent before rebooting
                        esp_restart();
                    }
                } else if (strncmp(token, "RES_EMPTY ", 10) == 0) {
                    double RES_EMPTY_UART;
                    if (sscanf(token, "RES_EMPTY %lf", &RES_EMPTY_UART) == 1) {
                        configManager.config.RES_EMPTY = RES_EMPTY_UART;
                        configManager.saveSettings();
                        uart_write_bytes(UART_NUM_1, "RES EMPTY Updated\n", strlen("RES EMPTY Updated\n")); // Send confirmation back to master
                        delay(2000); // Wait a moment to ensure the message is sent before rebooting
                        esp_restart();
                    }
                } else if (strncmp(token, "RES_LOW ", 8) == 0) {
                    double RES_LOW_UART;
                    if (sscanf(token, "RES_LOW %lf", &RES_LOW_UART) == 1) {
                        configManager.config.RES_LOW = RES_LOW_UART;
                        configManager.saveSettings();
                        uart_write_bytes(UART_NUM_1, "RES LOW Updated\n", strlen("RES LOW Updated\n")); // Send confirmation back to master
                        delay(2000); // Wait a moment to ensure the message is sent before rebooting
                        esp_restart();
                    }
                } else if (strncmp(token, "B2_PWR ", 7) == 0) {
                    unsigned long B2_PWR_UART;
                    if (sscanf(token + 7, "%lu", &B2_PWR_UART) == 1) {
                        configManager.config.B2_PWR = B2_PWR_UART;
                        configManager.saveSettings();
                        uart_write_bytes(UART_NUM_1, "B2 PWR Updated\n", strlen("B2 PWR Updated\n")); // Send confirmation back to master
                        delay(2000); // Wait a moment to ensure the message is sent before rebooting
                        esp_restart();
                    }
                } else if (strncmp(token, "B2_MAX_OT ", 10) == 0) {
                    unsigned long B2_MAX_OT_UART;
                    if (sscanf(token + 10, "%lu", &B2_MAX_OT_UART) == 1) {
                        configManager.config.B2_MAX_OT = B2_MAX_OT_UART;
                        configManager.saveSettings();
                        uart_write_bytes(UART_NUM_1, "B2 MAX OT Updated\n", strlen("B2 MAX OT Updated\n")); // Send confirmation back to master
                        delay(2000); // Wait a moment to ensure the message is sent before rebooting
                        esp_restart();
                    }
                } else if (strncmp(token, "B2_MIN_OT ", 10) == 0) {
                    unsigned long B2_MIN_OT_UART;
                    if (sscanf(token + 10, "%lu", &B2_MIN_OT_UART) == 1) {
                        configManager.config.B2_MIN_OT = B2_MIN_OT_UART;
                        configManager.saveSettings();
                        uart_write_bytes(UART_NUM_1, "B2 MIN OT Updated\n", strlen("B2 MIN OT Updated\n")); // Send confirmation back to master
                        delay(2000); // Wait a moment to ensure the message is sent before rebooting
                        esp_restart();
                    }
                } else if (strncmp(token, "B2_PWM ", 7) == 0) {
                    double B2_PWM_UART;
                    if (sscanf(token, "B2_PWM %lf", &B2_PWM_UART) == 1) {
                        configManager.config.B2_PWM = B2_PWM_UART;
                        configManager.saveSettings();
                        uart_write_bytes(UART_NUM_1, "B2 PWM Updated\n", strlen("B2 PWM Updated\n")); // Send confirmation back to master
                        delay(2000); // Wait a moment to ensure the message is sent before rebooting
                        esp_restart();
                    }
                } else if (strncmp(token, "B2_STABLE_PWM ", 14) == 0) {
                    unsigned long B2_STABLE_PWM_UART;
                    if (sscanf(token + 14, "%lu", &B2_STABLE_PWM_UART) == 1) {
                        configManager.config.B2_STABLE_PWM = B2_STABLE_PWM_UART;
                        configManager.saveSettings();
                        uart_write_bytes(UART_NUM_1, "B2 STABLE PWM Updated\n", strlen("B2 STABLE PWM Updated\n")); // Send confirmation back to master
                        delay(2000); // Wait a moment to ensure the message is sent before rebooting
                        esp_restart();
                    }
                } else if (strncmp(token, "B2_STABLE_TH ", 13) == 0) {
                    double B2_STABLE_TH_UART;
                    if (sscanf(token, "B2_STABLE_TH %lf", &B2_STABLE_TH_UART) == 1) {
                        configManager.config.B2_STABLE_TH = B2_STABLE_TH_UART;
                        configManager.saveSettings();
                        uart_write_bytes(UART_NUM_1, "B2 STABLE TH Updated\n", strlen("B2 STABLE TH Updated\n")); // Send confirmation back to master
                        delay(2000); // Wait a moment to ensure the message is sent before rebooting
                        esp_restart();
                    }
                } else if (strncmp(token, "PT2_MIN_V ", 10) == 0) {
                    double PT2_MIN_V_UART;
                    if (sscanf(token, "PT2_MIN_V %lf", &PT2_MIN_V_UART) == 1) {
                        configManager.config.PT2_MIN_V = PT2_MIN_V_UART;
                        configManager.saveSettings();
                        uart_write_bytes(UART_NUM_1, "PT2 MIN V Updated\n", strlen("PT2 MIN V Updated\n")); // Send confirmation back to master
                        delay(2000); // Wait a moment to ensure the message is sent before rebooting
                        esp_restart();
                    }
                } else if (strncmp(token, "PT2_MAX_V ", 10) == 0) {
                    double PT2_MAX_V_UART;
                    if (sscanf(token, "PT2_MAX_V %lf", &PT2_MAX_V_UART) == 1) {
                        configManager.config.PT2_MAX_V = PT2_MAX_V_UART;
                        configManager.saveSettings();
                        uart_write_bytes(UART_NUM_1, "PT2 MAX V Updated\n", strlen("PT2 MAX V Updated\n")); // Send confirmation back to master
                        delay(2000); // Wait a moment to ensure the message is sent before rebooting
                        esp_restart();
                    }
                } else if (strncmp(token, "PT2_KPA ", 8) == 0) {
                    double PT2_KPA_UART;
                    if (sscanf(token, "PT2_KPA %lf", &PT2_KPA_UART) == 1) {
                        configManager.config.PT2_KPA = PT2_KPA_UART;
                        configManager.saveSettings();
                        uart_write_bytes(UART_NUM_1, "PT2 KPA Updated\n", strlen("PT2 KPA Updated\n")); // Send confirmation back to master
                        delay(2000); // Wait a moment to ensure the message is sent before rebooting
                        esp_restart();
                    }
                } else if (strncmp(token, "B2_MIN_KPA ", 7) == 0) {
                    double B2_MIN_KPA_UART;
                    if (sscanf(token, "B2_MIN_KPA %lf", &B2_MIN_KPA_UART) == 1) {
                        configManager.config.B2_MIN_KPA = B2_MIN_KPA_UART;
                        configManager.saveSettings();
                        uart_write_bytes(UART_NUM_1, "B2 MIN KPA Updated\n", strlen("B2 MIN KPA Updated\n")); // Send confirmation back to master
                        delay(2000); // Wait a moment to ensure the message is sent before rebooting
                        esp_restart();
                    }
                } else if (strncmp(token, "B2_MAX_KPA ", 7) == 0) {
                    double B2_MAX_KPA_UART;
                    if (sscanf(token, "B2_MAX_KPA %lf", &B2_MAX_KPA_UART) == 1) {
                        configManager.config.B2_MAX_KPA = B2_MAX_KPA_UART;
                        configManager.saveSettings();
                        uart_write_bytes(UART_NUM_1, "B2 MAX KPA Updated\n", strlen("B2 MAX KPA Updated\n")); // Send confirmation back to master
                        delay(2000); // Wait a moment to ensure the message is sent before rebooting
                        esp_restart();
                    }
                } else if (strncmp(token, "TEMP_1_MIN ", 11) == 0) {
                    double TEMP_1_MIN_UART;
                    if (sscanf(token, "TEMP_1_MIN %lf", &TEMP_1_MIN_UART) == 1) {
                        configManager.config.TEMP_1_MIN = TEMP_1_MIN_UART;
                        configManager.saveSettings();
                        uart_write_bytes(UART_NUM_1, "TEMP 1 MIN Updated\n", strlen("TEMP 1 MIN Updated\n")); // Send confirmation back to master
                        delay(2000); // Wait a moment to ensure the message is sent before rebooting
                        esp_restart();
                    }
                } else if (strncmp(token, "TEMP_1_MAX ", 11) == 0) {
                    double TEMP_1_MAX_UART;
                    if (sscanf(token, "TEMP_1_MAX %lf", &TEMP_1_MAX_UART) == 1) {
                        configManager.config.TEMP_1_MAX = TEMP_1_MAX_UART;
                        configManager.saveSettings();
                        uart_write_bytes(UART_NUM_1, "TEMP 1 MAX Updated\n", strlen("TEMP 1 MAX Updated\n")); // Send confirmation back to master
                        delay(2000); // Wait a moment to ensure the message is sent before rebooting
                        esp_restart();
                    }
                } else if (strncmp(token, "TEMP_2_MIN ", 11) == 0) {
                    double TEMP_2_MIN_UART;
                    if (sscanf(token, "TEMP_2_MIN %lf", &TEMP_2_MIN_UART) == 1) {
                        configManager.config.TEMP_2_MIN = TEMP_2_MIN_UART;
                        configManager.saveSettings();
                        uart_write_bytes(UART_NUM_1, "TEMP 2 MIN Updated\n", strlen("TEMP 2 MIN Updated\n")); // Send confirmation back to master
                        delay(2000); // Wait a moment to ensure the message is sent before rebooting
                        esp_restart();
                    }
                } else if (strncmp(token, "TEMP_2_MAX ", 11) == 0) {
                    double TEMP_2_MAX_UART;
                    if (sscanf(token, "TEMP_2_MAX %lf", &TEMP_2_MAX_UART) == 1) {
                        configManager.config.TEMP_2_MAX = TEMP_2_MAX_UART;
                        configManager.saveSettings();
                        uart_write_bytes(UART_NUM_1, "TEMP 2 MAX Updated\n", strlen("TEMP 2 MAX Updated\n")); // Send confirmation back to master
                        delay(2000); // Wait a moment to ensure the message is sent before rebooting
                        esp_restart();
                    }
                } else if (strncmp(token, "RESTORE_DEFAULT", 12) == 0) {
                    configManager.writeDefaultSettings();

                // ENABLE/DISABLE SERIAL PRINTING                
                } else if (strcmp(token, "ENABLE_SERIAL_PRINTING") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.SERIAL_PRINTING = true;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "ENABLE SERIAL PRINTING\n", strlen("ENABLE SERIAL PRINTING\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();
                } else if (strcmp(token, "DISABLE_SERIAL_PRINTING") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.SERIAL_PRINTING = false;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "DISABLE SERIAL PRINTING\n", strlen("DISABLE SERIAL PRINTING\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();

                // ENABLE/DISABLE ULTRASONIC               
                } else if (strcmp(token, "ENABLE_ULTRASONIC_SENSOR") == 0) {
                    configManager.config.ULTRASONIC_SENSOR = true;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "ENABLED ULTRA SENSOR\n", strlen("ENABLED ULTRA SENSOR\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();
                } else if (strcmp(token, "DISABLE_ULTRASONIC_SENSOR") == 0) {
                    configManager.config.ULTRASONIC_SENSOR = false;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "DISABLED ULTRA SENSOR\n", strlen("DISABLED ULTRA SENSOR\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();

                // ENABLE/DISABLE TARGET TEMP B2               
                } else if (strcmp(token, "ENABLE_TARGET_TEMP_B2") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.TARGET_TEMP_B2 = true;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "TARGET TEMP B2 ENABLED\n", strlen("TARGET TEMP B2 ENABLED\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();
                } else if (strcmp(token, "DISABLE_TARGET_TEMP_B2") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.TARGET_TEMP_B2 = false;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "TARGET TEMP B2 DISABLED\n", strlen("TARGET TEMP B2 DISABLED\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();


                // ENABLE/DISABLE TEMP SENSOR 1      
                } else if (strcmp(token, "ENABLE_TEMP_SENSOR_1") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.TEMP_SENSOR_1 = true;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "ENABLED TEMP SENSOR 1\n", strlen("ENABLED TEMP SENSOR 1\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();
                } else if (strcmp(token, "DISABLE_TEMP_SENSOR_1") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.TEMP_SENSOR_1 = false;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "DISABLED TEMP SENSOR 1\n", strlen("DISABLED TEMP SENSOR 1\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();

                // ENABLE/DISABLE TEMP SENSOR 2  
                } else if (strcmp(token, "ENABLE_TEMP_SENSOR_2") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.TEMP_SENSOR_2 = true;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "ENABLED TEMP SENSOR 2\n", strlen("ENABLED TEMP SENSOR 2\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();
                } else if (strcmp(token, "DISABLE_TEMP_SENSOR_2") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.TEMP_SENSOR_2 = false;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "DISABLED TEMP SENSOR 2\n", strlen("DISABLED TEMP SENSOR 2\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();
                    
                // ENABLE/DISABLE PRESSURE TRANSDUCER 2 (TRANSDUCER_2)
                } else if (strcmp(token, "ENABLE_TRANSDUCER_2") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.TRANSDUCER_2 = true;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "ENABLED TRANSDUCER 2\n", strlen("ENABLED TRANSDUCER 2\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();
                } else if (strcmp(token, "DISABLE_TRANSDUCER_2") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.TRANSDUCER_2 = false;
                    configManager.config.TEMP_SENSOR_2 = true;
                    configManager.config.TARGET_TEMP_B2 = true;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "DISABLED TRANSDUCER 2\n", strlen("DISABLED TRANSDUCER 2\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();

                // ENABLE/DISABLE TRANSDUCER 2 KPA SAFE              
                } else if (strcmp(token, "ENABLE_TRANSDUCER_2_KPA_SAFE") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.TRANSDUCER_2_KPA_SAFE = true;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "ENABLE TRANSDUCER 2 KPA SAFE\n", strlen("ENABLE TRANSDUCER 2 KPA SAFE\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();
                } else if (strcmp(token, "DISABLE_TRANSDUCER_2_KPA_SAFE") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.TRANSDUCER_2_KPA_SAFE = false;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "DISABLED TRANSDUCER 2 KPA SAFE\n", strlen("DISABLED TRANSDUCER 2 KPA SAFE\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();
                    
                // ENABLE/DISABLE TEMP 1 SAFE            
                } else if (strcmp(token, "ENABLE_TEMP_1_SAFE") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.TEMP_1_SAFE = true;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "ENABLE TEMP 1 SAFE\n", strlen("ENABLE TEMP 1 SAFE\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();
                } else if (strcmp(token, "DISABLE_TEMP_1_SAFE") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.TEMP_1_SAFE = false;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "DISABLED TEMP 1 SAFE\n", strlen("DISABLED TEMP 1 SAFE\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();

                // ENABLE/DISABLE TEMP 2 SAFE                
                } else if (strcmp(token, "ENABLE_TEMP_2_SAFE") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.TEMP_2_SAFE = true;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "ENABLED TEMP 2 SAFE\n", strlen("ENABLED TEMP 2 SAFE\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();
                } else if (strcmp(token, "DISABLE_TEMP_2_SAFE") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.TEMP_2_SAFE = false;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "DISABLED TEMP 2 SAFE\n", strlen("DISABLED TEMP 2 SAFE\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();

                // ENABLE/DISABLE DUAL BOILER           
                } else if (strcmp(token, "ENABLE_DUAL_BOILER") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.DUAL_BOILER = true;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "ENABLE DUAL BOILER\n", strlen("ENABLE DUAL BOILER\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();
                } else if (strcmp(token, "DISABLE_DUAL_BOILER") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.DUAL_BOILER = false;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "DISABLED DUAL BOILER\n", strlen("DISABLED DUAL BOILER\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();

                // ENABLE/DISABLE PID CONTROL B2          
                } else if (strcmp(token, "ENABLE_PID_CONTROL_B2") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.PID_CONTROL_B2 = true;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "ENABLED PID CONTROL B2\n", strlen("ENABLED PID CONTROL B2\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();
                } else if (strcmp(token, "DISABLE_PID_CONTROL_B2") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.PID_CONTROL_B2 = false;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "DISABLED PID CONTROL B2\n", strlen("DISABLED PID CONTROL B2\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();

                // ENABLE/DISABLE RESERVOIR SWITCH     
                } else if (strcmp(token, "ENABLE_RES_SW") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.WTR_RES_PROBE = false;
                    configManager.config.US_LVL_DETECT = false;
                    configManager.config.RES_SW = true;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "ENABLE RES SW\n", strlen("ENABLE RES SW\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();
                } else if (strcmp(token, "DISABLE_RES_SW") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.RES_SW = false;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "DISABLED RES SW\n", strlen("DISABLED RES SW\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();
                
                // ENABLE/DISABLE RESERVOIR SWITCH NOMALLY CLOSED        
                } else if (strcmp(token, "ENABLE_RES_SW_NC") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.RES_SW_NC = true;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "ENABLE RES SW NC\n", strlen("ENABLE RES SW NC\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();
                } else if (strcmp(token, "DISABLE_RES_SW_NC") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.RES_SW_NC = false;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "DISABLED RES SW NC\n", strlen("DISABLED RES SW NC\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();

                // ENABLE/DISABLE WATER RESERVOIR PROBE          
                } else if (strcmp(token, "ENABLE_WTR_RES_PROBE") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.US_LVL_DETECT = false;
                    configManager.config.RES_SW = false;
                    configManager.config.WTR_RES_PROBE = true;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "ENABLE WTR RES PROBE\n", strlen("ENABLE WTR RES PROBE\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();
                } else if (strcmp(token, "DISABLE_WTR_RES_PROBE") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.WTR_RES_PROBE = false;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "DISABLED WTR RES PROBE\n", strlen("DISABLED WTR RES PROBE\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();


                // ENABLE/DISABLE ULTRASONIC SENSOR WATER LEVEL DETECTION         
                } else if (strcmp(token, "ENABLE_US_LVL_DETECT") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.RES_SW = false;
                    configManager.config.WTR_RES_PROBE = false;
                    configManager.config.US_LVL_DETECT = true;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "ENABLE US LVL DETECT\n", strlen("ENABLE US LVL DETECT\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();
                } else if (strcmp(token, "DISABLE_US_LVL_DETECT") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.US_LVL_DETECT = false;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "DISABLED US LVL DETECT\n", strlen("DISABLED US LVL DETECT\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();

                // ENABLE/DISABLE BOILER 1 WATER PROBE       
                } else if (strcmp(token, "ENABLE_B1_WTR_PROBE") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.B1_WTR_PROBE = true;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "ENABLE B1 WTR PROBE\n", strlen("ENABLE B1 WTR PROBE\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();
                } else if (strcmp(token, "DISABLE_B1_WTR_PROBE") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.B1_WTR_PROBE = false;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "DISABLED B1 WTR PROBE\n", strlen("DISABLED B1 WTR PROBE\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();

                // ENABLE/DISABLE BOILER 2 WATER PROBE       
                } else if (strcmp(token, "ENABLE_B2_WTR_PROBE") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.B2_WTR_PROBE = true;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "ENABLE B2 WTR PROBE\n", strlen("ENABLE B2 WTR PROBE\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();
                } else if (strcmp(token, "DISABLE_B2_WTR_PROBE") == 0) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.B2_WTR_PROBE = false;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, "DISABLED B2 WTR PROBE\n", strlen("DISABLED B2 WTR PROBE\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();

                // REBOOT
                } else if (strcmp(token, "REBOOT") == 0) {
                    uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n")); // Send confirmation back to master
                    delay(2000); // Wait a moment to ensure the message is sent before rebooting
                    esp_restart();
                }
                
                token = strtok(NULL, "\n");
            }
            memset(processingBuffer, 0, UART_RX_BUF_SIZE);
        }

        // Check for heartbeat timeout
        if (xTaskGetTickCount() - lastHeartbeatReceived > pdMS_TO_TICKS(HEARTBEAT_RECV_TIMEOUT_MS)) {
            Serial.println("Error: Heartbeat not received from master ESP32 after 6 seconds");
            currentError = LOST_UART_CONNECTION;
            ENTER_SAFE_MODE = true; // Set the error flag             
        }

        vTaskDelay(pdMS_TO_TICKS(20)); // Small delay to avoid task hogging CPU
    }

}