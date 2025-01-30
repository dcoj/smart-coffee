#include "HEADER.h"

// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================

void SERIAL_INPUT_TASK(void *pvParameters) {
    for (;;) {
        if (Serial.available() > 0) {
            String input = Serial.readStringUntil('\n');
            input.trim();

            double TempDoubleValue; // Temporary variable for parsed values
            int TempIntValue; // Temporary variable

            // CALIBRATION FOR LOAD CELL
            if (input == "CALIBRATE_LOAD_CELL") {
                if (configManager.config.LOAD_CELL) {
                    CALIBRATION_LOAD_CELL();
                } else {
                    Serial.println("Error! LOAD_CELL not enabled\nEnable the LOAD_CELL before attempting to calibrate");
                }

            // CALIBRATION FOR BOILER 1 PROBE
            } else if (input == "CALIBRATE_B1_PROBE") {
                if (configManager.config.B1_WTR_PROBE) {
                    CALIBRATION_B1_PROBE();
                } else {
                    Serial.println("Error! B1_WTR_PROBE not enabled\nEnable B1_WTR_PROBE before attempting to calibrate");
                }

            // CALIBRATION FOR BOILER 2 PROBE
            } else if (input == "CALIBRATE_B2_PROBE") {
                if (configManager.config.B2_WTR_PROBE) {
                    CALIBRATION_B2_PROBE();
                } else {
                    Serial.println("Error! B2_WTR_PROBE not enabled\nEnable B2_WTR_PROBE before attempting to calibrate");
                }

            // CALIBRATION FOR RESERVOIR PROBE
            } else if (input == "CALIBRATE_RES_PROBE") {
                if (configManager.config.WTR_RES_PROBE) {
                    CALIBRATION_RES_PROBE();
                } else {
                    Serial.println("Error! WTR_RES_PROBE not enabled\nEnable WTR_RES_PROBE before attempting to calibrate");
                }

            // CALIBRATION FOR PT1
            } else if (input == "CALIBRATE_PT1") {
                if (configManager.config.TRANSDUCER_1) {
                    CALIBRATION_PT1();
                } else {
                    Serial.println("Error! TRANSDUCER_1 not enabled\nEnable TRANSDUCER_1 before attempting to calibrate");
                }

            // CALIBRATION FOR PT2
            } else if (input == "CALIBRATE_PT2") {
                if (configManager.config.TRANSDUCER_2) {
                    CALIBRATION_PT2();
                } else {
                    Serial.println("Error! TRANSDUCER_2 not enabled\nEnable TRANSDUCER_2 before attempting to calibrate");
                }

            //================================================
            // LOGIC TO FORWARD SERIAL INPUT COMMANDS TO SLAVE
            //================================================

            // Print settings in serial monitor
            } else if (input == "PRINT_SETTINGS") {
                //uart_write_bytes(UART_NUM_1, "PRINT_SETTINGS\n", strlen("PRINT_SETTINGS\n")); // Send request to retrieve settings from slave esp32
                delay(200);
                PRINT_SETTINGS_M(); // Print the master's settings

                continue; // Skip the rest of the loop iteration
            } else if (input.startsWith("B2_KPA ")) {
                TempDoubleValue = input.substring(strlen("B2_KPA ")).toDouble();
                // Check if the value is within the acceptable range
                if (TempDoubleValue >= 9 && TempDoubleValue <= 201) {
                    String command = input + "\n";
                    configManager.config.B2_KPA = TempDoubleValue;
                    configManager.saveSettings();
                    uart_write_bytes(UART_NUM_1, command.c_str(), command.length());
                } else {
                    Serial.println("Error: B2_KPA value must be between 10 and 200.");
                }
            } else if (input.startsWith("B2_TEMP ")) {
                TempDoubleValue = input.substring(strlen("B2_TEMP ")).toDouble();
                // Check if the value is within the acceptable range
                if (TempDoubleValue >= 9 && TempDoubleValue <= 151) {
                    configManager.config.B2_TEMP = TempDoubleValue;
                    configManager.saveSettings();
                    String command = input + "\n";
                    uart_write_bytes(UART_NUM_1, command.c_str(), command.length());
                } else {
                    Serial.println("Error: B2_TEMP value must be between 10 and 150.");
                }

            } else if (input.startsWith("B2_KP ")) {
                configManager.config.B2_KP = input.substring(strlen("B2_KP ")).toDouble();
                configManager.saveSettings();
                String command = input + "\n";
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length());

            } else if (input.startsWith("B2_KI ")) {
                configManager.config.B2_KI = input.substring(strlen("B2_KI ")).toDouble();
                configManager.saveSettings();
                String command = input + "\n";
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length());

            } else if (input.startsWith("B2_KD ")) {
                configManager.config.B2_KD = input.substring(strlen("B2_KD ")).toDouble();
                configManager.saveSettings();
                String command = input + "\n";
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length());

            } else if (input.startsWith("RES_PROBE ")) {
                configManager.config.RES_PROBE = input.substring(strlen("RES_PROBE ")).toDouble();
                configManager.saveSettings();
                String command = input + "\n";
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length());

            } else if (input.startsWith("RES_FULL ")) {
                configManager.config.RES_FULL = input.substring(strlen("RES_FULL ")).toDouble();
                configManager.saveSettings();
                String command = input + "\n";
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length());

            } else if (input.startsWith("RES_EMPTY ")) {
                configManager.config.RES_EMPTY = input.substring(strlen("RES_EMPTY ")).toDouble();
                configManager.saveSettings();
                String command = input + "\n";
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length());

            } else if (input.startsWith("RES_LOW ")) {
                configManager.config.RES_LOW = input.substring(strlen("RES_LOW ")).toDouble();
                configManager.saveSettings();
                String command = input + "\n";
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length());

            } else if (input.startsWith("B2_PWR ")) {
                String valueStr = input.substring(strlen("B2_PWR "));
                unsigned long value = strtoul(valueStr.c_str(), NULL, 10);
                configManager.config.B2_PWR = value;
                configManager.saveSettings();
                String command = input + "\n";
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length());

            } else if (input.startsWith("B2_MAX_OT ")) {
                String valueStr = input.substring(strlen("B2_MAX_OT "));
                unsigned long value = strtoul(valueStr.c_str(), NULL, 10);
                configManager.config.B2_MAX_OT = value;
                configManager.saveSettings();
                String command = input + "\n";
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length());

            } else if (input.startsWith("B2_MIN_OT ")) {
                String valueStr = input.substring(strlen("B2_MIN_OT "));
                unsigned long value = strtoul(valueStr.c_str(), NULL, 10);
                configManager.config.B2_MIN_OT = value;
                configManager.saveSettings();
                String command = input + "\n";
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length());

            } else if (input.startsWith("B2_PWM ")) {
                configManager.config.B2_PWM = input.substring(strlen("B2_PWM ")).toDouble();
                configManager.saveSettings();
                String command = input + "\n";
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length());

            } else if (input.startsWith("B2_STABLE_PWM ")) {
                String valueStr = input.substring(strlen("B2_STABLE_PWM "));
                unsigned long value = strtoul(valueStr.c_str(), NULL, 10);
                configManager.config.B2_STABLE_PWM = value;
                configManager.saveSettings();
                String command = input + "\n";
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length());

            } else if (input.startsWith("B2_STABLE_TH ")) {
                configManager.config.B2_STABLE_TH = input.substring(strlen("B2_STABLE_TH ")).toDouble();
                configManager.saveSettings();
                String command = input + "\n";
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length());

            } else if (input.startsWith("PT2_MIN_V ")) {
                configManager.config.PT2_MIN_V = input.substring(strlen("PT2_MIN_V ")).toDouble();
                configManager.saveSettings();
                String command = input + "\n";
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length());

            } else if (input.startsWith("PT2_MAX_V ")) {
                configManager.config.PT2_MAX_V = input.substring(strlen("PT2_MAX_V ")).toDouble();
                configManager.saveSettings();
                String command = input + "\n";
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length());

            } else if (input.startsWith("PT2_KPA ")) {
                configManager.config.PT2_KPA = input.substring(strlen("PT2_KPA ")).toDouble();
                configManager.saveSettings();
                String command = input + "\n";
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length());

            } else if (input.startsWith("B2_MIN_KPA ")) {
                TempDoubleValue = input.substring(strlen("B2_MIN_KPA ")).toDouble();
                 // Check if the value is within the acceptable range
                if (TempIntValue >= -21 && TempIntValue <= 1) {
                    configManager.config.B2_MIN_KPA = input.substring(strlen("B2_MIN_KPA ")).toDouble();
                    configManager.saveSettings();
                    String command = input + "\n";
                    uart_write_bytes(UART_NUM_1, command.c_str(), command.length());
                } else {
                    Serial.println("Error: B2_MIN_KPA value must be between -20 and 0.");
                }
            } else if (input.startsWith("B2_MAX_KPA ")) {
                TempDoubleValue = input.substring(strlen("B2_MAX_KPA ")).toDouble();
                 // Check if the value is within the acceptable range
                if (TempDoubleValue >= 9 && TempDoubleValue <= 201) {
                    configManager.config.B2_MAX_KPA = input.substring(strlen("B2_MAX_KPA ")).toDouble();
                    configManager.saveSettings();
                    String command = input + "\n";
                    uart_write_bytes(UART_NUM_1, command.c_str(), command.length());
                } else {
                    Serial.println("Error: B2_MAX_KPA value must be between 10 and 200.");
                }
            // UPDATE TEMP 2 MIN/MAX
            } else if (input.startsWith("TEMP_2_MIN ")) {
                TempDoubleValue = input.substring(strlen("TEMP_2_MIN ")).toDouble();
                 // Check if the value is within the acceptable range
                if (TempDoubleValue >= 0 && TempDoubleValue <= 21) { // Check if within range
                    configManager.config.TEMP_2_MIN = TempDoubleValue;
                    configManager.saveSettings();
                    String command = input + "\n";
                    uart_write_bytes(UART_NUM_1, command.c_str(), command.length());
                } else {
                    Serial.println("Error: TEMP_2_MIN value must be between 1 and 20.");
                }
             } else if (input.startsWith("TEMP_2_MAX ")) {
                TempDoubleValue = input.substring(strlen("TEMP_2_MAX ")).toDouble();
                 // Check if the value is within the acceptable range
                if (TempDoubleValue >= 29 && TempDoubleValue <= 150) { // Check if within range
                    configManager.config.TEMP_2_MAX = TempDoubleValue;
                    configManager.saveSettings();
                    String command = input + "\n";
                    uart_write_bytes(UART_NUM_1, command.c_str(), command.length());
                } else {
                    Serial.println("Error: TEMP_2_MAX value must be between 30 and 150.");
                }
            // ENABLE/DISABLE TARGET TEMP B2
            } else if (input == "ENABLE_TARGET_TEMP_B2") {
                configManager.config.TARGET_TEMP_B2 = true;
                configManager.config.TEMP_SENSOR_2 = true;
                configManager.saveSettings();
                String command = "ENABLE_TARGET_TEMP_B2\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32
            } else if (input == "DISABLE_TARGET_TEMP_B2") {
                configManager.config.TARGET_TEMP_B2 = false;
                configManager.config.TRANSDUCER_2 = true;
                configManager.saveSettings();
                Serial.println("Target Temp B2 DISABLED\nAUTO_ENABLING: TRANSDUCER 2");
                String command = "DISABLE_TARGET_TEMP_B2\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32

            // UPDATE TEMP 1 MIN/MAX
            } else if (input.startsWith("TEMP_1_MIN ")) {
                TempDoubleValue = input.substring(strlen("TEMP_1_MIN ")).toDouble();
                 // Check if the value is within the acceptable range
                if (TempDoubleValue >= 0 && TempDoubleValue <= 21) { // Check if within range
                    configManager.config.TEMP_1_MIN = TempDoubleValue;
                    configManager.saveSettings();
                    String command = input + "\n";
                    uart_write_bytes(UART_NUM_1, command.c_str(), command.length());
                } else {
                    Serial.println("Error: TEMP_1_MIN value must be between 1 and 20.");
                }
             } else if (input.startsWith("TEMP_1_MAX ")) {
                TempDoubleValue = input.substring(strlen("TEMP_1_MAX ")).toDouble();
                 // Check if the value is within the acceptable range
                if (TempDoubleValue >= 29 && TempDoubleValue <= 150) { // Check if within range
                    configManager.config.TEMP_1_MAX = TempDoubleValue;
                    configManager.saveSettings();
                    String command = input + "\n";
                    uart_write_bytes(UART_NUM_1, command.c_str(), command.length());
                } else {
                    Serial.println("Error: TEMP_1_MAX value must be between 30 and 150.");
                }

            // ENABLE/DISABLE RESERVOIR SWITCH
            } else if (input == "ENABLE_RES_SW") {
                configManager.config.RES_SW = true;
                configManager.saveSettings();
                String command = "ENABLE_RES_SW\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32
            } else if (input == "DISABLE_RES_SW") {
                configManager.config.RES_SW = false;
                configManager.saveSettings();
                String command = "DISABLE_RES_SW\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32

            // ENABLE/DISABLE RESERVOIR SWITCH NOMALLY CLOSED
            } else if (input == "ENABLE_RES_SW_NC") {
                configManager.config.RES_SW_NC = true;
                configManager.saveSettings();
                String command = "ENABLE_RES_SW_NC\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32
            } else if (input == "DISABLE_RES_SW_NC") {
                configManager.config.RES_SW_NC = false;
                configManager.saveSettings();
                String command = "DISABLE_RES_SW_NC\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32

            // ENABLE/DISABLE WATER RESERVOIR PROBE
            } else if (input == "ENABLE_WTR_RES_PROBE") {
                configManager.config.WTR_RES_PROBE = true;
                configManager.saveSettings();
                String command = "ENABLE_WTR_RES_PROBE\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32
            } else if (input == "DISABLE_WTR_RES_PROBE") {
                configManager.config.WTR_RES_PROBE = false;
                configManager.saveSettings();
                String command = "DISABLE_WTR_RES_PROBE\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32

            // ENABLE/DISABLE INVERT WATER RESERVOIR PROBE DIRECTION
            } else if (input == "ENABLE_INVERT_WTR_RES_DIR") {
                String command = "ENABLE_INVERT_WTR_RES_DIR\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32
            } else if (input == "DISABLE_INVERT_WTR_RES_DIR") {
                String command = "DISABLE_INVERT_WTR_RES_DIR\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32

            // ENABLE/DISABLE ULTRASONIC SENSOR WATER LEVEL DETECTION
            } else if (input == "ENABLE_US_LVL_DETECT") {
                configManager.config.US_LVL_DETECT = true;
                configManager.config.ULTRASONIC_SENSOR = true;
                configManager.config.WTR_RES_PROBE = false;
                configManager.config.RES_SW = false;
                configManager.saveSettings();
                String command = "ENABLE_US_LVL_DETECT\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32
            } else if (input == "DISABLE_US_LVL_DETECT") {
                configManager.config.US_LVL_DETECT = false;
                configManager.saveSettings();
                String command = "DISABLE_US_LVL_DETECT\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32

            //==============================================================================================================
            // LOGIC TO UPDATE LITTLEFS VALUES VIA SERIAL INPUT
            //==============================================================================================================
            
            // RESTORE DEFAULTS
            } else if (input == "RESTORE_DEFAULTS") {
                Serial.println("Resetting to default settings...");
                uart_write_bytes(UART_NUM_1, "RESTORE_DEFAULT\n", strlen("RESTORE_DEFAULT\n")); // Send msg to slave to restore default values
                configManager.writeDefaultSettings(); // Restore default values for master
                Serial.println("Default values restored");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // BOILER 1 PROBE
            } else if (input.startsWith("B1_REFILL_TRIG ")) {
                configManager.config.B1_REFILL_TRIG = input.substring(strlen("B1_REFILL_TRIG ")).toDouble();
                configManager.saveSettings();
                Serial.println("B1_REFILL_TRIG updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // BOILER 2 PROBE
            } else if (input.startsWith("B2_REFILL_TRIG ")) {
                String command = input + "\n";
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length());
                configManager.config.B2_REFILL_TRIG = input.substring(strlen("B2_REFILL_TRIG ")).toDouble();
                configManager.saveSettings();
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));


            // B1 KPA
            } else if (input.startsWith("B1_KPA ")) {
                TempDoubleValue = input.substring(strlen("B1_KPA ")).toDouble();
                if (TempDoubleValue >= 9 && TempDoubleValue <= 201) { // Check if within range
                    configManager.config.B1_KPA = TempDoubleValue;
                    configManager.saveSettings();
                    Serial.println("B1_KPA updated");
                    uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
                } else {
                    Serial.println("Error: B1_KPA value must be between 10 and 200");
                }


            // B1 TEMP
            } else if (input.startsWith("B1_TEMP ")) {
                TempDoubleValue = input.substring(strlen("B1_TEMP ")).toDouble();
                if (TempDoubleValue >= 9 && TempDoubleValue <= 151) { // Check if within range
                    configManager.config.B1_TEMP = TempDoubleValue;
                    configManager.saveSettings();
                    Serial.println("B1_TEMP updated");
                    uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
                } else {
                    Serial.println("Error: B1_TEMP value must be between 10 and 150");
                }
            

            // B1 KP
            } else if (input.startsWith("B1_KP ")) {
                configManager.config.B1_KP = input.substring(strlen("B1_KP ")).toDouble();
                configManager.saveSettings();
                Serial.println("B1_KP updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // B1 KI
            } else if (input.startsWith("B1_KI ")) {
                configManager.config.B1_KI = input.substring(strlen("B1_KI ")).toDouble();
                configManager.saveSettings();
                Serial.println("B1_KI updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
            
            // B1 KD
            } else if (input.startsWith("B1_KD ")) {
                configManager.config.B1_KD = input.substring(strlen("B1_KD ")).toDouble();
                configManager.saveSettings();
                Serial.println("B1_KD updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // SHOT ML
            } else if (input.startsWith("SHOT_ML ")) {
                configManager.config.SHOT_ML = input.substring(strlen("SHOT_ML ")).toDouble();
                configManager.saveSettings();
                Serial.println("SHOT_ML updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
            
            // SHOT GRAMS
            } else if (input.startsWith("SHOT_GRAM ")) {
                configManager.config.SHOT_GRAM = input.substring(strlen("SHOT_GRAM ")).toDouble();
                configManager.saveSettings();
                Serial.println("SHOT_GRAM updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // WEIGHT OFFSET
            } else if (input.startsWith("WEIGHT_OFFSET ")) {
                configManager.config.WEIGHT_OFFSET = input.substring(strlen("WEIGHT_OFFSET ")).toDouble();
                configManager.saveSettings();
                Serial.println("WEIGHT_OFFSET updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // SCALE FACTOR
            } else if (input.startsWith("SCALE_FACTOR ")) {
                configManager.config.SCALE_FACTOR = input.substring(strlen("SCALE_FACTOR ")).toDouble();
                configManager.saveSettings();
                Serial.println("SCALE_FACTOR updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // FM1 HZ
            } else if (input.startsWith("FM1_HZ ")) {
                configManager.config.FM1_HZ = input.substring(strlen("FM1_HZ ")).toDouble();
                configManager.saveSettings();
                Serial.println("FM1_HZ updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
            
            // FM2 HZ
            } else if (input.startsWith("FM2_HZ ")) {
                configManager.config.FM2_HZ = input.substring(strlen("FM2_HZ ")).toDouble();
                configManager.saveSettings();
                Serial.println("FM2_HZ updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // SHOT TIMER
            } else if (input.startsWith("SHOT_TIMER ")) {
                String valueStr = input.substring(strlen("SHOT_TIMER "));
                unsigned long value = strtoul(valueStr.c_str(), NULL, 10);
                configManager.config.SHOT_TIMER = value;
                configManager.saveSettings();
                Serial.println("SHOT_TIMER updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));


            // PRIME PUMP
            } else if (input.startsWith("PRIME_PUMP ")) {
                String valueStr = input.substring(strlen("PRIME_PUMP "));
                unsigned long value = strtoul(valueStr.c_str(), NULL, 10);
                configManager.config.PRIME_PUMP = value;
                configManager.saveSettings();
                Serial.println("PRIME_PUMP updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // INFUSION HZ
            } else if (input.startsWith("INFUSION_HZ ")) {
                String valueStr = input.substring(strlen("INFUSION_HZ "));
                unsigned long value = strtoul(valueStr.c_str(), NULL, 10);
                configManager.config.INFUSION_HZ = value;
                configManager.saveSettings();
                Serial.println("INFUSION_HZ updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // INFUSION TIMER
            } else if (input.startsWith("INFUSION_TIMER ")) {
                String valueStr = input.substring(strlen("INFUSION_TIMER "));
                unsigned long value = strtoul(valueStr.c_str(), NULL, 10);
                configManager.config.INFUSION_TIMER = value;
                configManager.saveSettings();
                Serial.println("INFUSION_TIMER updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // PT1 MIN VOLTAGE
            } else if (input.startsWith("PT1_MIN_V ")) {
                configManager.config.PT1_MIN_V = input.substring(strlen("PT1_MIN_V ")).toDouble();
                configManager.saveSettings();
                Serial.println("PT1_MIN_V updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // PT1 MAX VOLTAGE
            } else if (input.startsWith("PT1_MAX_V")) {
                configManager.config.PT1_MAX_V = input.substring(strlen("PT1_MAX_V ")).toDouble();
                configManager.saveSettings();
                Serial.println("PT1_MAX_V updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));


            // PT1 KPA TARGET
            } else if (input.startsWith("PT1_KPA ")) {
                configManager.config.PT1_KPA = input.substring(strlen("PT1_KPA ")).toDouble();
                configManager.saveSettings();
                Serial.println("PT1_KPA updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // B1 PWR
            } else if (input.startsWith("B1_PWR ")) {
                String valueStr = input.substring(strlen("B1_PWR "));
                unsigned long value = strtoul(valueStr.c_str(), NULL, 10);
                configManager.config.B1_PWR = value;
                configManager.saveSettings();
                Serial.println("B1_PWR updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // B1 MAX OFF-TIME
            } else if (input.startsWith("B1_MAX_OT ")) {
                String valueStr = input.substring(strlen("B1_MAX_OT "));
                unsigned long value = strtoul(valueStr.c_str(), NULL, 10);
                configManager.config.B1_MAX_OT = value;
                configManager.saveSettings();
                Serial.println("B1_MAX_OT updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // B1 MIN OFF-TIME
            } else if (input.startsWith("B1_MIN_OT ")) {
                String valueStr = input.substring(strlen("B1_MIN_OT "));
                unsigned long value = strtoul(valueStr.c_str(), NULL, 10);
                configManager.config.B1_MIN_OT = value;
                configManager.saveSettings();
                Serial.println("B1_MIN_OT updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // B1 PWM
            } else if (input.startsWith("B1_PWM ")) {
                configManager.config.B1_PWM = input.substring(strlen("B1_PWM ")).toDouble();
                configManager.saveSettings();
                Serial.println("B1_PWM updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // B1 STABLE PWM
            } else if (input.startsWith("B1_STABLE_PWM ")) {
                String valueStr = input.substring(strlen("B1_STABLE_PWM "));
                unsigned long value = strtoul(valueStr.c_str(), NULL, 10);
                configManager.config.B1_STABLE_PWM = value;
                configManager.saveSettings();
                Serial.println("B1_STABLE_PWM updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // B1 STABLE THRESHOLD
            } else if (input.startsWith("B1_STABLE_TH ")) {
                configManager.config.B1_STABLE_TH = input.substring(strlen("B1_STABLE_TH ")).toDouble();
                configManager.saveSettings();
                Serial.println("B1_STABLE_TH updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // TRANSDUCER 1 MIN KPA
            } else if (input.startsWith("B1_MIN_KPA ")) {
                TempDoubleValue = input.substring(strlen("B1_MIN_KPA ")).toDouble();
                if (TempDoubleValue >= -21 && TempDoubleValue <= 1) { // Check if within range
                    configManager.config.B1_MIN_KPA = TempDoubleValue;
                    configManager.saveSettings();
                    Serial.println("B1_MIN_KPA updated");
                    uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
                } else {
                    Serial.println("B1_MIN_KPA value must be between -20 and 0");
                }

            // TRANSDUCER 1 MAX KPA
            } else if (input.startsWith("B1_MAX_KPA ")) {
                TempDoubleValue = input.substring(strlen("B1_MAX_KPA ")).toDouble();
                if (TempDoubleValue >= 9 && TempDoubleValue <= 201) { // Check if within range
                    configManager.config.B1_MAX_KPA = TempDoubleValue;
                    configManager.saveSettings();
                    Serial.println("B1_MAX_KPA updated");
                    uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
                } else {
                    Serial.println("B1_MAX_KPA value must be between 10 and 200");
                }
            // B1 REFILL TIMER
            } else if (input.startsWith("B1_REFILL_TIMER ")) {
                String valueStr = input.substring(strlen("B1_REFILL_TIMER "));
                unsigned long value = strtoul(valueStr.c_str(), NULL, 10);
                configManager.config.B1_REFILL_TIMER = value;
                configManager.saveSettings();
                Serial.println("B1_REFILL_TIMER updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

                // B2 REFILL TIMER
            } else if (input.startsWith("B2_REFILL_TIMER ")) {
                String valueStr = input.substring(strlen("B2_REFILL_TIMER "));
                unsigned long value = strtoul(valueStr.c_str(), NULL, 10);
                configManager.config.B2_REFILL_TIMER = value;
                configManager.saveSettings();
                Serial.println("B2_REFILL_TIMER updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // RESERVOIR REFILL TIMER
            } else if (input.startsWith("RES_REFILL_TIMER ")) {
                String valueStr = input.substring(strlen("RES_REFILL_TIMER "));
                unsigned long value = strtoul(valueStr.c_str(), NULL, 10);
                configManager.config.RES_REFILL_TIMER = value;
                configManager.saveSettings();
                Serial.println("RES_REFILL_TIMER updated");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // ENABLE/DISABLE SERIAL PRINTING
            } else if (input == "ENABLE_SERIAL_PRINTING") {
                configManager.config.SERIAL_PRINTING = true;
                configManager.saveSettings();
                uart_write_bytes(UART_NUM_1, "ENABLE_SERIAL_PRINTING\n", strlen("ENABLE_SERIAL_PRINTING\n"));
            } else if (input == "DISABLE_SERIAL_PRINTING") {
                configManager.config.SERIAL_PRINTING = false;
                configManager.saveSettings();
                uart_write_bytes(UART_NUM_1, "DISABLE_SERIAL_PRINTING\n", strlen("DISABLE_SERIAL_PRINTING\n"));

            // ENABLE/DISABLE ULTRASONIC SENSOR
            } else if (input == "ENABLE_ULTRASONIC_SENSOR") {
                String command = "ENABLE_ULTRASONIC_SENSOR\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32
                configManager.config.ULTRASONIC_SENSOR = true;
                configManager.saveSettings();
            } else if (input == "DISABLE_ULTRASONIC_SENSOR") {
                String command = "DISABLE_ULTRASONIC_SENSOR\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32
                configManager.config.ULTRASONIC_SENSOR = false;
                configManager.saveSettings();

            // ENABLE/DISABLE USE IMPERIAL UNITS
            } else if (input == "ENABLE_USE_IMPERIAL_UNITS") {
                configManager.config.USE_IMPERIAL_UNITS = true;
                configManager.saveSettings();
                Serial.println("Imperial units enabled.");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
            } else if (input == "DISABLE_USE_IMPERIAL_UNITS") {
                configManager.config.USE_IMPERIAL_UNITS = false;
                configManager.saveSettings();
                Serial.println("Imperial units disabled.");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // ENABLE/DISABLE ESPRESSO VOLUME
            } else if (input == "ENABLE_ESPRESSO_VOLUME") {
                configManager.config.ESPRESSO_WEIGHT = false;
                configManager.config.ESPRESSO_TIMER = false;
                configManager.config.ESPRESSO_VOLUME = true;
                configManager.config.FLOW_METER = true;
                configManager.saveSettings();
                Serial.println("Espresso volume enabled.");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
            } else if (input == "DISABLE_ESPRESSO_VOLUME") {
                configManager.config.ESPRESSO_VOLUME = false;
                configManager.saveSettings();
                Serial.println("Espresso volume disabled.");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // ENABLE/DISABLE ESPRESSO WEIGHT
            } else if (input == "ENABLE_ESPRESSO_WEIGHT") {
                configManager.config.ESPRESSO_VOLUME = false;
                configManager.config.ESPRESSO_TIMER = false;
                configManager.config.ESPRESSO_WEIGHT = true;
                configManager.config.LOAD_CELL = true;
                configManager.saveSettings();
                Serial.println("Espresso weight enabled.");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
            } else if (input == "DISABLE_ESPRESSO_WEIGHT") {
                configManager.config.ESPRESSO_WEIGHT = false;
                configManager.saveSettings();
                Serial.println("Espresso weight disabled.");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // ENABLE/DISABLE ESPRESSO TIMER
            } else if (input == "ENABLE_ESPRESSO_TIMER") {
                configManager.config.ESPRESSO_WEIGHT = false;
                configManager.config.ESPRESSO_VOLUME = false;
                configManager.config.ESPRESSO_TIMER = true;
                configManager.saveSettings();
                Serial.println("Espresso timer enabled.");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
            } else if (input == "DISABLE_ESPRESSO_TIMER") {
                configManager.config.ESPRESSO_TIMER = false;
                configManager.saveSettings();
                Serial.println("Espresso timer disabled.");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // ENABLE/DISABLE FLOW METER
            } else if (input == "ENABLE_FLOW_METER") {
                STOP_TASKS ();
                delay(1000);
                configManager.config.FLOW_METER = true;
                configManager.config.ESPRESSO_VOLUME = true;
                configManager.config.ESPRESSO_WEIGHT = false;
                configManager.config.ESPRESSO_TIMER = false;
                configManager.config.LOAD_CELL = false; // Disable load cell. Flow meter and load cell CANNOT be both enabled at the same time
                configManager.saveSettings();
                Serial.println("Flow meter enabled");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
            } else if (input == "DISABLE_FLOW_METER") {
                STOP_TASKS ();
                delay(1000);
                configManager.config.FLOW_METER = false;
                configManager.config.ESPRESSO_VOLUME = false;
                configManager.saveSettings();
                Serial.println("Flow meter disabled");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // ENABLE/DISABLE FLOW METER 2
            } else if (input == "ENABLE_FLOW_METER_2") {
                if (configManager.config.FLOW_METER) {
                    STOP_TASKS ();
                    delay(1000);
                    configManager.config.FLOW_METER = true;
                    configManager.config.FLOW_METER_2 = true;
                    configManager.saveSettings();
                    Serial.println("Flow meter 2 enabled");
                    uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
                } else {
                    Serial.println("Error! Flow Meter must be enabled before Flow Meter 2");
                }
            } else if (input == "DISABLE_FLOW_METER_2") {
                STOP_TASKS ();
                delay(1000);
                configManager.config.FLOW_METER_2 = false;
                configManager.saveSettings();
                Serial.println("Flow meter 2 disabled");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // ENABLE/DISABLE LOAD_CELL
            } else if (input == "ENABLE_LOAD_CELL") {
                STOP_TASKS ();
                delay(1000);
                configManager.config.LOAD_CELL = true;
                configManager.config.ESPRESSO_WEIGHT = true;
                configManager.config.FLOW_METER = false; // Disable flow meter.  Flow meter and load cell CANNOT be both enabled at the same time
                configManager.config.FLOW_METER_2 = false; // Disable flow meter 2
                configManager.config.ESPRESSO_VOLUME = false; //  Disable shot volume
                configManager.config.ESPRESSO_TIMER = false; // Disable shot timer
                configManager.saveSettings();
                Serial.println("Load cell enabled");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
            } else if (input == "DISABLE_LOAD_CELL") {
                STOP_TASKS ();
                delay(1000);
                configManager.config.LOAD_CELL = false;
                configManager.config.ESPRESSO_WEIGHT = false;
                configManager.saveSettings();
                Serial.println("Load cell disabled");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // ENABLE/DISABLE DUAL BOILER
            } else if (input == "ENABLE_DUAL_BOILER") {
                String command = "ENABLE_DUAL_BOILER\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32
                STOP_TASKS ();
                delay(1000);
                configManager.config.DUAL_BOILER = true;
                configManager.saveSettings();
                Serial.println("Please make sure that TRANSDUCER_2 or TARGET_TEMP_B2 is enabled to function correctly");
            } else if (input == "DISABLE_DUAL_BOILER") {
                String command = "DISABLE_DUAL_BOILER\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32
                STOP_TASKS ();
                delay(1000);
                configManager.config.DUAL_BOILER = false;
                configManager.saveSettings();

            // ENABLE/DISABLE TARGET TEMP B1
            } else if (input == "ENABLE_TARGET_TEMP_B1") {
                STOP_TASKS ();
                delay(1000);
                configManager.config.TARGET_TEMP_B1 = true;
                configManager.config.TEMP_SENSOR_1 = true;
                configManager.saveSettings();
                Serial.println("Target Temp B1 ENABLED");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
            } else if (input == "DISABLE_TARGET_TEMP_B1") {
                STOP_TASKS ();
                delay(1000);
                configManager.config.TARGET_TEMP_B1 = false;
                configManager.config.TRANSDUCER_1 = true;
                configManager.saveSettings();
                Serial.println("Target Temp B1 DISABLED\nAUTO_ENABLING: TRANSDUCER 1");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // ENABLE/DISABLE TEMP SENSOR 1
            } else if (input == "ENABLE_TEMP_SENSOR_1") {
                String command = "ENABLE_TEMP_SENSOR_1\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32
                STOP_TASKS ();
                delay(1000);
                configManager.config.TEMP_SENSOR_1 = true;
                configManager.saveSettings();
            } else if (input == "DISABLE_TEMP_SENSOR_1") {
                String command = "DISABLE_TEMP_SENSOR_1\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32
                STOP_TASKS ();
                delay(1000);
                configManager.config.TEMP_SENSOR_1 = false;
                configManager.saveSettings();

            // ENABLE/DISABLE TEMP SENSOR 2
            } else if (input == "ENABLE_TEMP_SENSOR_2") {
                String command = "ENABLE_TEMP_SENSOR_2\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32
                STOP_TASKS ();
                delay(1000);
                configManager.config.TEMP_SENSOR_2 = true;
                configManager.saveSettings();
            } else if (input == "DISABLE_TEMP_SENSOR_2") {
                String command = "DISABLE_TEMP_SENSOR_2\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32
                STOP_TASKS ();
                delay(1000);
                configManager.config.TEMP_SENSOR_2 = false;
                configManager.saveSettings();

            // ENABLE/DISABLE TRANSDUCER 1
            } else if (input == "ENABLE_TRANSDUCER_1") {
                STOP_TASKS ();
                delay(1000);
                configManager.config.TRANSDUCER_1 = true;
                configManager.saveSettings();
                Serial.println("Pressure Transducer 1 ENABLED");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
            } else if (input == "DISABLE_TRANSDUCER_1") {
                String command = "ENABLE_TEMP_SENSOR_1\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32
                STOP_TASKS ();
                delay(1000);
                configManager.config.TRANSDUCER_1 = false;
                configManager.config.TARGET_TEMP_B1 = true;
                configManager.config.TEMP_SENSOR_1 = true;
                configManager.saveSettings();
                Serial.println("Pressure Transducer 1 DISABLED.\nAUTO-ENABLING: Temp Sensor 1, Target Temp B1");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // ENABLE/DISABLE TRANSDUCER 2
            } else if (input == "ENABLE_TRANSDUCER_2") {
                String command = "ENABLE_TRANSDUCER_2\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32
                STOP_TASKS ();
                delay(1000);
                configManager.config.TRANSDUCER_2 = true;
                configManager.saveSettings();
            } else if (input == "DISABLE_TRANSDUCER_2") {
                String command = "DISABLE_TRANSDUCER_2\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32
                STOP_TASKS ();
                delay(1000);
                configManager.config.TRANSDUCER_2 = false;
                configManager.config.TARGET_TEMP_B2 = true;
                configManager.config.TEMP_SENSOR_2 = true;
                Serial.println("Pressure Transducer 2 DISABLED.\nAUTO-ENABLING: Temp Sensor 2, Target Temp B2");
                configManager.saveSettings();

            // ENABLE/DISABLE TRANSDUCER 1 KPA SAFE (TRANSDUCER_1_KPA_SAFE)
            } else if (input == "ENABLE_TRANSDUCER_1_KPA_SAFE") {
                STOP_TASKS ();
                delay(1000);
                configManager.config.TRANSDUCER_1_KPA_SAFE = true;
                configManager.config.TRANSDUCER_1 = true;
                configManager.saveSettings();
                Serial.println("Transducer 1 kPa Safe ENABLED");
                delay(2000);
                esp_restart();
            } else if (input == "DISABLE_TRANSDUCER_1_KPA_SAFE") {
                STOP_TASKS ();
                delay(1000);
                configManager.config.TRANSDUCER_1_KPA_SAFE = false;
                configManager.saveSettings();
                Serial.println("Transducer 1 kPa Safe DISABLED");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // ENABLE/DISABLE TRANSDUCER 2 KPA SAFE  
            } else if (input == "ENABLE_TRANSDUCER_2_KPA_SAFE") {
                configManager.config.TRANSDUCER_2_KPA_SAFE = true;
                configManager.config.TRANSDUCER_2 = true;
                configManager.saveSettings();
                String command = "ENABLE_TRANSDUCER_2_KPA_SAFE\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32
            } else if (input == "DISABLE_TRANSDUCER_2_KPA_SAFE") {
                configManager.config.TRANSDUCER_2_KPA_SAFE = false;
                configManager.saveSettings();
                String command = "DISABLE_TRANSDUCER_2_KPA_SAFE\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32

            // ENABLE/DISABLE TEMP 1 SAFE   
            } else if (input == "ENABLE_TEMP_1_SAFE") {
                configManager.config.TEMP_1_SAFE = true;
                configManager.config.TEMP_SENSOR_1 = true;
                configManager.saveSettings();
                String command = "ENABLE_TEMP_1_SAFE\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32
            } else if (input == "DISABLE_TEMP_1_SAFE") {
                configManager.config.TEMP_1_SAFE = false;
                configManager.saveSettings();
                String command = "DISABLE_TEMP_1_SAFE\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32

            // ENABLE/DISABLE TEMP 2 SAFE   
            } else if (input == "ENABLE_TEMP_2_SAFE") {
                configManager.config.TEMP_2_SAFE = true;
                configManager.config.TEMP_SENSOR_2 = true;
                configManager.saveSettings();
                String command = "ENABLE_TEMP_2_SAFE\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32
            } else if (input == "DISABLE_TEMP_2_SAFE") {
                configManager.config.TEMP_2_SAFE = false;
                configManager.saveSettings();
                String command = "DISABLE_TEMP_2_SAFE\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32

            // ENABLE/DISABLE PRE INFUSION
            } else if (input == "ENABLE_PRE_INFUSION") {
                STOP_TASKS ();
                delay(1000);
                configManager.config.PRE_INFUSION = true;
                configManager.saveSettings();
                Serial.println("Pre infusion ENABLED");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
            } else if (input == "DISABLE_PRE_INFUSION") {
                STOP_TASKS ();
                delay(1000);
                configManager.config.PRE_INFUSION = false;
                configManager.saveSettings();
                Serial.println("Pre infusion DISABLED");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // ENABLE/DISABLE PID CONTROL B1
            } else if (input == "ENABLE_PID_CONTROL_B1") {
                STOP_TASKS ();
                delay(1000);
                configManager.config.PID_CONTROL_B1 = true;
                configManager.saveSettings();
                Serial.println("PID control B1 ENABLED.");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
            } else if (input == "DISABLE_PID_CONTROL_B1") {
                STOP_TASKS ();
                delay(1000);
                configManager.config.PID_CONTROL_B1 = false;
                configManager.saveSettings();
                Serial.println("PID control B1 DISABLED.");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // ENABLE/DISABLE PAUSE REFILL
            } else if (input == "ENABLE_PAUSE_REFILL") {
                STOP_TASKS ();
                delay(1000);
                configManager.config.PAUSE_REFILL = true;
                configManager.saveSettings();
                Serial.println("Pause Refill ENABLED.");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
            } else if (input == "DISABLE_PAUSE_REFILL") {
                STOP_TASKS ();
                delay(1000);
                configManager.config.PAUSE_REFILL = false;
                configManager.saveSettings();
                Serial.println("Pause Refill DISABLED.");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // ENABLE/DISABLE STARTUP LOGO
            } else if (input == "ENABLE_STARTUP_LOGO") {
                STOP_TASKS ();
                delay(1000);
                configManager.config.STARTUP_LOGO = true;
                configManager.saveSettings();
                Serial.println("Startup Logo ENABLED.");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
            } else if (input == "DISABLE_STARTUP_LOGO") {
                STOP_TASKS ();
                delay(1000);
                configManager.config.STARTUP_LOGO = false;
                configManager.saveSettings();
                Serial.println("Startup Logo DISABLED.");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // ENABLE/DISABLE FLOW METER 2 CALIBRATION MODE
            } else if (input == "ENABLE_FLOW_METER_2_CAL") {
                STOP_TASKS ();
                delay(1000);
                configManager.config.FLOW_METER = true;
                configManager.config.FLOW_METER_2 = true;
                configManager.config.FLOW_METER_2_CAL = true;
                configManager.config.SHOT_ML = 40;
                configManager.saveSettings();
                Serial.println("Flow Meter 2 Calibration mode ENABLED. Setting SHOT_ML value to 40");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
            } else if (input == "DISABLE_FLOW_METER_2_CAL") {
                STOP_TASKS ();
                delay(1000);
                configManager.config.FLOW_METER_2_CAL = false;
                configManager.saveSettings();
                Serial.println("Flow Meter 2 Calibration mode DISABLED. REMINDER: To set your SHOT_ML value after calibration ");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // ENABLE/DISABLE PID CONTROL B2
            } else if (input == "ENABLE_PID_CONTROL_B2") {
                configManager.config.PID_CONTROL_B2 = true;
                configManager.saveSettings();
                uart_write_bytes(UART_NUM_1, "ENABLE_PID_CONTROL_B2\n", strlen("ENABLE_PID_CONTROL_B2\n"));
            } else if (input == "DISABLE_PID_CONTROL_B2") {
                configManager.config.PID_CONTROL_B2 = false;
                configManager.saveSettings();
                uart_write_bytes(UART_NUM_1, "DISABLE_PID_CONTROL_B2\n", strlen("DISABLE_PID_CONTROL_B2\n"));

            // ENABLE/DISABLE TOGGLE BREW SWITCH
            } else if (input == "ENABLE_TOGGLE_BREW_SW") {
                configManager.config.TOGGLE_BREW_SW = true;
                configManager.saveSettings();
                Serial.println(" Brew Switch enabled.");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
            } else if (input == "DISABLE_TOGGLE_BREW_SW") {
                configManager.config.TOGGLE_BREW_SW = false;
                configManager.saveSettings();
                Serial.println("Toggle Brew Switch disabled.");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // ENABLE/DISABLE TOGGLE ESPRESSO MODE SWITCH
            } else if (input == "ENABLE_TOGGLE_ES_SW") {
                configManager.config.TOGGLE_ES_SW = true;
                configManager.saveSettings();
                Serial.println("Toggle Espresso Mode Switch enabled.");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
            } else if (input == "DISABLE_TOGGLE_ES_SW") {
                configManager.config.TOGGLE_ES_SW = false;
                configManager.saveSettings();
                Serial.println("Toggle Espresso Switch disabled.");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // ENABLE/DISABLE BOILER 1 WATER PROBE
            } else if (input == "ENABLE_B1_WTR_PROBE") {
                configManager.config.B1_WTR_PROBE = true;
                configManager.saveSettings();
                Serial.println("Boiler 1 Water Probe ENABLED.");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
            } else if (input == "DISABLE_B1_WTR_PROBE") {
                configManager.config.B1_WTR_PROBE = false;
                configManager.saveSettings();
                Serial.println("Boiler 1 Water Probe DISABLED.");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // ENABLE/DISABLE BOILER 2 WATER PROBE
            } else if (input == "ENABLE_B2_WTR_PROBE") {
                configManager.config.B2_WTR_PROBE = true;
                configManager.saveSettings();
                Serial.println("Boiler 2 Water Probe ENABLED.");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
            } else if (input == "DISABLE_B2_WTR_PROBE") {
                configManager.config.B2_WTR_PROBE = false;
                configManager.saveSettings();
                Serial.println("Boiler 2 Water Probe DISABLED.");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // ENABLE/DISABLE RELAYS
            } else if (input == "ENABLE_RELAYS") {
                String command = "ENABLE_RELAYS\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32
                configManager.config.DISABLE_RELAYS = false;
                configManager.saveSettings();
                Serial.println("RELAYS ENABLED.");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));
            } else if (input == "DISABLE_RELAYS") {
                String command = "DISABLE_RELAYS\n"; // Message to send to Slave ESP32
                uart_write_bytes(UART_NUM_1, command.c_str(), command.length()); // Forward this command to the Slave ESP32
                configManager.config.DISABLE_RELAYS = true;
                configManager.saveSettings();
                Serial.println("RELAYS DISABLED.");
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));


            // Print Master's esp32 settings only in serial monitor
            } else if (input.startsWith("PRINT_MASTER")) {
                PRINT_SETTINGS_M();
            // Print Slave's esp32 settings only in serial monitor
            } else if (input.startsWith("PRINT_SLAVE")) {
                uart_write_bytes(UART_NUM_1, "PRINT_SETTINGS\n", strlen("PRINT_SETTINGS\n")); // Send request to retreive settings from slave esp32
                Serial.println("Sent request to retrieve slave settings");
                continue; // Skip the rest of the loop iteration
                
            // Reboot device
            } else if (input.startsWith("REBOOT")) {
                uart_write_bytes(UART_NUM_1, "REBOOT\n", strlen("REBOOT\n"));

            // Information for changing settings
            } else if (input.startsWith("INFO")) {
                Serial.println("========================================================================================\n"
                              "========================================================================================\n"
                              "NOTE: Commands must be sent in UPPERCASE letters\n"
                              "\n"
                              "List of commands:\n"
                              "PRINT_SETTINGS = prints the current stored settings\n"
                              "PRINT_SLAVE = prints only the slave ESP32 settings\n"
                              "REBOOT = reboots the operating system\n"
                              "RESTORE_DEFAULTS = restores default settings and values\n"
                              "ENABLE_SERIAL_PRINTING = enables serial printing of several sensor values\n"
                              "\n"
                              "To modify a value, enter the value's name followed by a space then your chosen number, for example: SHOT_TIMER 10\n"
                              "\n"
                              "To enable OR disable a feature, enter ENABLE_ or DISABLE_ followed by the name, for example ENABLE_PRE_INFUSION or DISABLE_PRE_INFUSION\n"
                              "\n"
                              "CALIBRATE_LOAD_CELL = Starts the calibration wizard for the load cell\n"
                              "CALIBRATE_B1_PROBE = Starts the calibration wizard for the Boiler 1 Probe\n"
                              "CALIBRATE_B2_PROBE = Starts the calibration wizard for the Boiler 2 Probe\n"
                              "CALIBRATE_RES_PROBE = Starts the calibration wizard for the Reservoior Probe\n"
                              "CALIBRATE_PT1 = Starts the calibration wizard for Pressure Transducer 1\n"
                              "CALIBRATE_PT2 = Starts the calibration wizard for Pressure Transducer 2\n"
                              "BUG reporting: If you have found an issue or bug with the program, send a description of the problem and your configuration to mail@Schematix.co.nz\n"
                              "========================================================================================\n"
                              "========================================================================================\n");
            }

            // Handle unknown input
            else {
                Serial.println("Command not recognized. Please check your message and try again.\nTo learn more about commands, send command: INFO");
            }


        }
        vTaskDelay(pdMS_TO_TICKS(500)); // Check every 500 ms
    }
}
