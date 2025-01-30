#include "HEADER.h"

// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================

void UART_TASK(void *pvParameters) {
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(HEARTBEAT_SEND_INTERVAL_MS);
    xLastWakeTime = xTaskGetTickCount();
    TickType_t lastHeartbeatReceived = xTaskGetTickCount();

    bool receivingSettings = false; // flag to indicate serial printing for incoming json file settings

    if (ENTER_SAFE_MODE) {
        uart_write_bytes(UART_NUM_1, "ENTER_SM\n", strlen("ENTER_SM\n"));
        delay(1500);
        vTaskSuspend(NULL);  // Permanently suspend this task
    }

    while (1) {
        // ============================
        // ==== OUTGOING UART DATA ====
        // ============================

        // Send heartbeat every 2 seconds
        if (xTaskGetTickCount() - xLastWakeTime >= xFrequency) {
            uart_write_bytes(UART_NUM_1, "HEARTBEAT\n", strlen("HEARTBEAT\n")); // Include newline
            xLastWakeTime = xTaskGetTickCount();
        }

        // Send reboot message to slave
        if (REBOOT) {
            uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
        }

        // Send calibration mode message to slave
        if (CALIBRATING_B2) {
            uart_write_bytes(UART_NUM_1, "CALIBRATING_B2\n", strlen("CALIBRATING_B2\n"));
        }

        // Handle SEND_PT2_SETTINGS message to send PT2 settings to slave
        if (SEND_PT2_SETTINGS) {
            SEND_PT2_SETTINGS = false; // Reset the flag
            uart_write_bytes(UART_NUM_1, "PT2_INCOMING\n", strlen("PT2_INCOMING\n"));
            sendPT2Settings();
        }
        
        // ============================
        // ==== INCOMING UART DATA ====
        // ============================
        int len = uart_read_bytes(UART_NUM_1, activeBuffer, UART_RX_BUF_SIZE - 1, 20 / portTICK_RATE_MS);
        if (len > 0) {
            activeBuffer[len] = '\0'; // Null-terminate the received data
            SW_BUFFERS(); // Switch the active and processing buffers

            // Uncomment to serial print incoming UART data
            //Serial.println("Received: ");
            //Serial.println((char *)processingBuffer);

            char *token = strtok((char *)processingBuffer, "\n"); //
            while (token != NULL) {

                // HEATBEAT UART CONNECTION
                if (strcmp(token, "HEARTBEAT") == 0) {
                    lastHeartbeatReceived = xTaskGetTickCount();
                    //printf("Heartbeat received from slave ESP32\n");

                // Handle BOOT_SETUP message to send shared settings to slave
                } else if (strcmp(token, "BOOT_SETUP") == 0) {
                    SEND_BOOT_SETUP();
                
                // Handle PT2_VOLTAGE message from slave
                } else if (strncmp(token, "PT2_VOLTAGE:", 12) == 0) {
                    PT2_VOLTAGE = atof(token + 12); // Extract voltage value and update PT2_VOLTAGE

                // Handle RESERVOIR PROBE VALUE message from slave
                } else if (strncmp(token, "RES_TOUCH:", 10) == 0) {
                    RES_TOUCH_VALUE = atof(token + 10); // Extract voltage value and update RES_TOUCH_VALUE


                // MESSAGE TO SERIAL PRINT SENSOR DATA
                } else if (strcmp(token, "MSPS") == 0) {
                    if (configManager.config.TRANSDUCER_1) {
                        // Pressure sensor data
                        int rawValue = analogRead(PRESSURE_SENSOR_PIN);
                        double voltage = rawValue * (5.0 / 4095.0);
                        Serial.print("PT1 Volts: ");
                        Serial.print(voltage);
                    }
                    if (configManager.config.FLOW_METER) {
                        Serial.print(" | FM1: ");
                        Serial.print(FM1_PULSE_COUNT);       
                    }

                    if (configManager.config.FLOW_METER_2) {
                        Serial.print(" | FM2: ");
                        Serial.print(FM2_PULSE_COUNT);       
                    }
                    
                // LOGIC TO PRINT SETTINGS FROM SLAVE ESP32
                } else if (strncmp(token, "SETTINGS_START", 14) == 0) {
                    //Serial.println("Receiving settings from slave...");
                    receivingSettings = true; // Start receiving settings, turn the flag on
                } else if (strncmp(token, "SETTINGS_END", 12) == 0) {
                    //Serial.println("Settings received from slave.");
                    receivingSettings = false; // Finished receiving settings, turn the flag off
                } else if (receivingSettings) {
                    Serial.println(token); // Print each line of the settings
                }
                // RECEIVE ULTRASONIC WATER PERCENTAGE
                if (strncmp(token, "UWP:", 4) == 0) {
                    int receivedWaterPercentage;
                    if (sscanf(token, "UWP:%d", &receivedWaterPercentage) == 1) {
                        // Update ULTRA_WATER_PERCENTAGE under mutex protection
                        if (xMutexWaterResPercentage != NULL) {
                            if (xSemaphoreTake(xMutexWaterResPercentage, (TickType_t)10) == pdTRUE) {
                                ULTRA_WATER_PERCENTAGE = receivedWaterPercentage;
                                xSemaphoreGive(xMutexWaterResPercentage);
                            }
                        }
                    }
                // RECEIVE TEMP 1 VALUE
                } else if (strncmp(token, "TEMP1:", 6) == 0) {
                    double LOCAL_TEMP_1;
                    if (sscanf(token, "TEMP1:%lf", &LOCAL_TEMP_1) == 1) {
                        // Update TEMP_C_1 under mutex protection
                        if (xMutexTemprature1 != NULL) {
                            if (xSemaphoreTake(xMutexTemprature1, (TickType_t)10) == pdTRUE) {
                                TEMP_C_1 = LOCAL_TEMP_1;
                                xSemaphoreGive(xMutexTemprature1);
                            }
                        }
                    }
                // RECEIVE TEMP 2 VALUE
                } else if (strncmp(token, "TEMP2:", 6) == 0) {
                    double LOCAL_TEMP_2;
                    if (sscanf(token, "TEMP2:%lf", &LOCAL_TEMP_2) == 1) {
                        // Update TEMP_C_2 under mutex protection
                        if (xMutexTemprature2 != NULL) {
                            if (xSemaphoreTake(xMutexTemprature2, (TickType_t)10) == pdTRUE) {
                                TEMP_C_2 = LOCAL_TEMP_2;
                                xSemaphoreGive(xMutexTemprature2);
                            }
                        }
                    }

                // RECEIVE BOILER 1 TOUCH VALUE
                } else if (strncmp(token, "B1_TOUCH:", 9) == 0) {
                    int receivedB1TouchValue;
                    if (sscanf(token, "B1_TOUCH:%d", &receivedB1TouchValue) == 1) {
                        B1_TOUCH_VALUE = receivedB1TouchValue;
                    }

                // RECEIVE BOILER 2 TOUCH VALUE
                } else if (strncmp(token, "B2_TOUCH:", 9) == 0) {
                    int receivedB2TouchValue;
                    if (sscanf(token, "B2_TOUCH:%d", &receivedB2TouchValue) == 1) {
                        B2_TOUCH_VALUE = receivedB2TouchValue;
                    }
                    

                // RECEIVE RESERVOIR REFILL
                } else if (strcmp(token, "REFILL_RESERVOIR") == 0) {
                    RESERVOIR_REFILL_REQUIRED = true; // Set the flag to true
                // RECEIVE RESERVOIR FULL
                } else if (strcmp(token, "RESERVOIR_FULL") == 0) {
                    RESERVOIR_REFILL_REQUIRED = false;

                // RECEIVE PRESSURE TRANSDUCER 2 kPa value
                } else if (strncmp(token, "KPA2:", 5) == 0) {
                    double LOCAL_PRESSURE_KPA_2;
                    if (sscanf(token, "KPA2:%lf", &LOCAL_PRESSURE_KPA_2) == 1) {
                        // Update PRESSURE_KPA_2 under mutex protection
                        if (xMutexPressure2 != NULL) {
                            if (xSemaphoreTake(xMutexPressure2, (TickType_t)10) == pdTRUE) {
                                PRESSURE_KPA_2 = LOCAL_PRESSURE_KPA_2;
                                xSemaphoreGive(xMutexPressure2);
                                //Serial.print(LOCAL_PRESSURE_KPA_2);
                            }
                        }
                    }
                    // RECEIVE ELEMENT 2 ON STATE
                    } else if (strcmp(token, "E2_ON") == 0) {
                        ELEMENT_2_STATE = true; // Set the flag to true

                    // RECEIVE ELEMENT 2 OFF STATE
                    } else if (strcmp(token, "E2_OFF") == 0) {
                        ELEMENT_2_STATE = false; // Set the flag to true

                    // RECEIVE BOILER 2 PRESSURE SAFE MODE
                    } else if (strcmp(token, "PT2_P_SM") == 0) {
                        currentError = TRANSDUCER_2_SM;
                        ENTER_SAFE_MODE = true; // Set the error flag

                    // RECEIVE TEMP SENSOR 1 SAFE MODE
                    } else if (strcmp(token, "TS_1_SM") == 0) {
                        currentError = TEMP_SENSOR_1_SM;
                        ENTER_SAFE_MODE = true; // Set the error flag

                    // RECEIVE TEMP SENSOR 2 SAFE MODE
                    } else if (strcmp(token, "TS_2_SM") == 0) {
                        currentError = TEMP_SENSOR_2_SM;
                        ENTER_SAFE_MODE = true; // Set the error flag
                        

          

                // Check for incoming confirmation message from slave
                } else if (strcmp(token, "B2 KPA Updated") == 0) {
                    Serial.println("B2 KPA updated. Rebooting...");
                    delay(2000);
                    esp_restart();

                } else if (strcmp(token, "B2 TEMP Updated") == 0) {
                    Serial.println("B2 TEMP updated. Rebooting...");
                    delay(2000);
                    esp_restart();

                } else if (strcmp(token, "B2 Kp Updated") == 0) {
                    Serial.println("B2 Kp updated. Rebooting...");
                    delay(2000);
                    esp_restart();

                } else if (strcmp(token, "B2 Ki Updated") == 0) {
                    Serial.println("B2 Ki updated. Rebooting...");
                    delay(2000);
                    esp_restart();

                } else if (strcmp(token, "B2 Kd Updated") == 0) {
                    Serial.println("B2 Kd updated. Rebooting...");
                    delay(2000);
                    esp_restart();

                } else if (strcmp(token, "RES PROBE Updated") == 0) {
                    Serial.println("RES PROBE updated. Rebooting...");
                    delay(2000);
                    esp_restart();

                } else if (strcmp(token, "RES FULL Updated") == 0) {
                    Serial.println("RES FULL updated. Rebooting...");
                    delay(2000);
                    esp_restart();

                } else if (strcmp(token, "RES EMPTY Updated") == 0) {
                    Serial.println("RES EMPTY updated. Rebooting...");
                    delay(2000);
                    esp_restart();

                } else if (strcmp(token, "RES LOW Updated") == 0) {
                    Serial.println("RES LOW updated. Rebooting...");
                    delay(2000);
                    esp_restart();

                } else if (strcmp(token, "B2 PWR Updated") == 0) {
                    Serial.println("B2 PWR updated. Rebooting...");
                    delay(2000);
                    esp_restart();

                } else if (strcmp(token, "B2 MAX OT Updated") == 0) {
                    Serial.println("B2 MAX OT updated. Rebooting...");
                    delay(2000);
                    esp_restart();

                } else if (strcmp(token, "B2 MIN OT Updated") == 0) {
                    Serial.println("B2 MIN OT updated. Rebooting...");
                    delay(2000);
                    esp_restart();
                } else if (strcmp(token, "B2 PWM Updated") == 0) {
                    Serial.println("B2 PWM updated. Rebooting...");
                    delay(2000);
                    esp_restart();

                } else if (strcmp(token, "B2 STABLE PWM Updated") == 0) {
                    Serial.println("B2 STABLE PWM updated. Rebooting...");
                    delay(2000);
                    esp_restart();

                } else if (strcmp(token, "B2 STABLE TH Updated") == 0) {
                    Serial.println("B2 STABLE TH updated. Rebooting...");
                    delay(2000);
                    esp_restart();

                } else if (strcmp(token, "PT2 MIN V Updated") == 0) {
                    Serial.println("PT2 MIN V updated. Rebooting...");
                    delay(2000);
                    esp_restart();

                } else if (strcmp(token, "PT2 MAX V Updated") == 0) {
                    Serial.println("PT2 MAX V updated. Rebooting...");
                    delay(2000);
                    esp_restart();

                } else if (strcmp(token, "PT2 KPA Updated") == 0) {
                    Serial.println("PT2 KPA updated. Rebooting...");
                    delay(2000);
                    esp_restart();

                } else if (strcmp(token, "B2 MIN KPA Updated") == 0) {
                    Serial.println("B2 MIN KPA updated. Rebooting...");
                    delay(2000);
                    esp_restart();

                } else if (strcmp(token, "B2 MAX KPA Updated") == 0) {
                    Serial.println("B2 MAX KPA pressure updated. Rebooting...");
                    delay(2000);
                    esp_restart();

                } else if (strcmp(token, "B2 TEMP MIN Updated") == 0) {
                    Serial.println("B2 TEMP MIN updated. Rebooting...");
                    delay(2000);
                    esp_restart();

                } else if (strcmp(token, "B2 TEMP MAX Updated") == 0) {
                    Serial.println("B2 TEMP MAX updated. Rebooting...");
                    delay(2000);
                    esp_restart();

                // UPDATED B2_PROBE
                } else if (strcmp(token, "B2 REFILL TRIG Updated") == 0) {
                    Serial.println("B2 PROBE VALUE UPDATED. Rebooting...");
                    delay(2000);
                    esp_restart();

                // ENABLE/DISABLE TARGET TEMP B2 
                } else if (strcmp(token, "TARGET TEMP B2 ENABLED") == 0) {
                    Serial.println("TARGET TEMP B2 ENABLED. Rebooting...");
                    delay(2000);
                    esp_restart();
                } else if (strcmp(token, "TARGET TEMP B2 DISABLED") == 0) {
                    Serial.println("TARGET TEMP B2 DISABLED. Rebooting...");
                    delay(2000);
                    esp_restart();
                    
                // ENABLE/DISABLE ULTRASONIC   
                } else if (strcmp(token, "ENABLED ULTRA SENSOR") == 0) {
                    Serial.println("ULTRASONIC SENSOR ENABLED. Rebooting...");
                    delay(2000);
                    esp_restart();
                } else if (strcmp(token, "DISABLED ULTRA SENSOR") == 0) {
                    Serial.println("Ultrasonic sensor DISABLED. Rebooting...");
                    delay(2000);
                    esp_restart();

                // ENABLE/DISABLE TEMP SENSOR 1  
                } else if (strcmp(token, "ENABLED TEMP SENSOR 1") == 0) {
                    Serial.println("Temp sensor 1 ENABLED. Rebooting...");
                    delay(2000);
                    esp_restart();
                } else if (strcmp(token, "DISABLED TEMP SENSOR 1") == 0) {
                    Serial.println("Temp sensor 1 DISABLED. Rebooting...");
                    delay(2000);
                    esp_restart();

                // ENABLE/DISABLE TEMP SENSOR 2  
                } else if (strcmp(token, "ENABLED TEMP SENSOR 2") == 0) {
                    Serial.println("Temp sensor 2 ENABLED. Rebooting...");
                    delay(2000);
                    esp_restart();
                } else if (strcmp(token, "DISABLED TEMP SENSOR 2") == 0) {
                    Serial.println("Temp sensor 2 DISABLED. Rebooting...");
                    delay(2000);
                    esp_restart();

                // ENABLE/DISABLE PRESSURE TRANSDUCER 2 (TRANSDUCER_2)  
                } else if (strcmp(token, "ENABLED TRANSDUCER 2") == 0) {
                    Serial.println("Pressure Transducer 2 ENABLED. Rebooting...");
                    delay(2000);
                    esp_restart();
                } else if (strcmp(token, "DISABLED TRANSDUCER 2") == 0) {
                    Serial.println("Pressure Transducer 2 DISABLED.\nENABLING: Temp Sensor 2, Target Temp B2\nRebooting...");
                    delay(3000);
                    esp_restart();

                // ENABLE/DISABLE TRANSDUCER 2 KPA SAFE
                } else if (strcmp(token, "ENABLE TRANSDUCER 2 KPA SAFE") == 0) {
                    Serial.println("Transducer 2 kPa Safe ENABLED. Rebooting...");
                    delay(2000);
                    esp_restart();
                } else if (strcmp(token, "DISABLED TRANSDUCER 2 KPA SAFE") == 0) {
                    Serial.println("Transducer 2 kPa Safe DISABLED. Rebooting...");
                    delay(2000);
                    esp_restart();

                // ENABLE/DISABLE TEMP 1 SAFE 
                } else if (strcmp(token, "ENABLE TEMP 1 SAFE") == 0) {
                    Serial.println("Temp 1 Safe ENABLED. Rebooting...");
                    delay(2000);
                    esp_restart();
                } else if (strcmp(token, "DISABLED TEMP 1 SAFE") == 0) {
                    Serial.println("Temp 1 Safe DISABLED. Rebooting...");
                    delay(2000);
                    esp_restart();

                // ENABLE/DISABLE TEMP 2 SAFE 
                } else if (strcmp(token, "ENABLED TEMP 2 SAFE") == 0) {
                    Serial.println("Temp 2 Safe ENABLED. Rebooting...");
                    delay(2000);
                    esp_restart();
                } else if (strcmp(token, "DISABLED TEMP 2 SAFE") == 0) {
                    Serial.println("Temp 2 Safe DISABLED. Rebooting...");
                    delay(2000);
                    esp_restart();

                // UPDATE TEMP 1 MIN/MAX SAFE VALUES   
                } else if (strcmp(token, "TEMP 1 MIN Updated") == 0) {
                    Serial.println("Temp 1 Min UPDATED. Rebooting...");
                    delay(2000);
                    esp_restart();
                } else if (strcmp(token, "TEMP 1 MAX Updated") == 0) {
                    Serial.println("Temp 1 Max UPDATED. Rebooting...");
                    delay(2000);
                    esp_restart();

                // UPDATE TEMP 2 MIN/MAX SAFE VALUES      
                } else if (strcmp(token, "TEMP 2 MIN Updated") == 0) {
                    Serial.println("Temp 2 Min UPDATED. Rebooting...");
                    delay(2000);
                    esp_restart();
                } else if (strcmp(token, "TEMP 2 MAX Updated") == 0) {
                    Serial.println("Temp 2 Max UPDATED. Rebooting...");
                    delay(2000);
                    esp_restart();

                // ENABLE/DISABLE DUAL BOILER
                } else if (strcmp(token, "ENABLE DUAL BOILER") == 0) {
                    Serial.println("ENABLED DUAL BOILER. Rebooting...");
                    delay(2000);
                    esp_restart();
                } else if (strcmp(token, "DISABLED DUAL BOILER") == 0) {
                    Serial.println("DISABLED DUAL BOILER. Rebooting...");
                    delay(2000);
                    esp_restart();

                // ENABLE/DISABLE PID CONTROL B2
                } else if (strcmp(token, "ENABLED PID CONTROL B2") == 0) {
                    Serial.println("PID CONTROL B2 ENABLED. Rebooting...");
                    delay(2000);
                    esp_restart();
                } else if (strcmp(token, "DISABLED PID CONTROL B2") == 0) {
                    Serial.println("PID CONTROL B2 DISABLED. Rebooting...");
                    delay(2000);
                    esp_restart();
                

                // ENABLE/DISABLE RESERVOIR SWITCH   
                } else if (strcmp(token, "ENABLE RES SW") == 0) {
                    Serial.println("ENABLED RESERVOIR SWITCH. Rebooting...");
                    delay(2000);
                    esp_restart();
                } else if (strcmp(token, "DISABLED DISABLED RES SW") == 0) {
                    Serial.println("DISABLED RESERVOIR SWITCH. Rebooting...");
                    delay(2000);
                    esp_restart();

                // ENABLE/DISABLE RESERVOIR SWITCH NOMALLY CLOSED
                } else if (strcmp(token, "ENABLE RES SW NC") == 0) {
                    Serial.println("ENABLED RESERVOIR SWITCH NORMALLY CLOSED. Rebooting...");
                    delay(2000);
                    esp_restart();
                } else if (strcmp(token, "DISABLED RES SW NC") == 0) {
                    Serial.println("DISABLED RESERVOIR SWITCH NORMALLY CLOSED. Rebooting...");
                    delay(2000);
                    esp_restart();


                // ENABLE/DISABLE WATER RESERVOIR PROBE
                } else if (strcmp(token, "ENABLE WTR RES PROBE") == 0) {
                    Serial.println("ENABLED WATER RESERVOIR PROBE. Rebooting...");
                    delay(2000);
                    esp_restart();
                } else if (strcmp(token, "DISABLED WTR RES PROBE") == 0) {
                    Serial.println("DISABLED WATER RESERVOIR PROBE. Rebooting...");
                    delay(2000);
                    esp_restart();

                // ENABLE/DISABLE ULTRASONIC SENSOR WATER LEVEL DETECTION
                } else if (strcmp(token, "ENABLE US LVL DETECT") == 0) {
                    Serial.println("ENABLED ULTRASONIC SENSOR LEVEL DETECT. Rebooting...");
                    delay(2000);
                    esp_restart();
                } else if (strcmp(token, "DISABLED US LVL DETECT") == 0) {
                    Serial.println("DISABLED ULTRASONIC SENSOR LEVEL DETECT. Rebooting...");
                    delay(2000);
                    esp_restart();

                // ENABLE/DISABLE SERIAL PRINTING
                } else if (strcmp(token, "ENABLE SERIAL PRINTING") == 0) {
                    Serial.println("Serial Printing ENABLED. Rebooting...");
                    delay(2000);
                    esp_restart();
                } else if (strcmp(token, "DISABLE SERIAL PRINTING") == 0) {
                    Serial.println("Serial Printing DISABLED. Rebooting...");
                    delay(2000);
                    esp_restart();

                // ENABLE/DISABLE BOILER 1 WATER PROBE  
                } else if (strcmp(token, "ENABLE B1 WTR PROBE") == 0) {
                    Serial.println("ENABLED BOILER 1 WATER PROBE. Rebooting...");
                    delay(2000);
                    esp_restart();
                } else if (strcmp(token, "DISABLED B1 WTR PROBE") == 0) {
                    Serial.println("DISABLED BOILER 1 WATER PROBE. Rebooting...");
                    delay(2000);
                    esp_restart();

                // ENABLE/DISABLE BOILER 2 WATER PROBE  
                } else if (strcmp(token, "ENABLE B2 WTR PROBE") == 0) {
                    Serial.println("ENABLED BOILER 2 WATER PROBE. Rebooting...");
                    delay(2000);
                    esp_restart();
                } else if (strcmp(token, "DISABLED B2 WTR PROBE") == 0) {
                    Serial.println("DISABLED BOILER 2 WATER PROBE. Rebooting...");
                    delay(2000);
                    esp_restart();

                // REBOOT
                } else if (strcmp(token, "REBOOT") == 0) {
                    Serial.println("Rebooting...");
                    delay(2000);
                    esp_restart();
                }
                token = strtok(NULL, "\n");
            }
            memset(processingBuffer, 0, UART_RX_BUF_SIZE);
        }

        // Check for heartbeat timeout 
        if (xTaskGetTickCount() - lastHeartbeatReceived > pdMS_TO_TICKS(HEARTBEAT_RECV_TIMEOUT_MS)) {
            Serial.println("Error: Heartbeat not received from slave ESP32 after 6 seconds");
            currentError = LOST_UART_CONNECTION;
            ENTER_SAFE_MODE = true; // Set the error flag 
        }

        vTaskDelay(pdMS_TO_TICKS(20)); // Small delay to avoid task hogging CPU
    }

}