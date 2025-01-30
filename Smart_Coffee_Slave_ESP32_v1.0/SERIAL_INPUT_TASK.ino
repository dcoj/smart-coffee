#include "HEADER.h"

// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================

void SERIAL_INPUT_TASK(void *pvParameters) {
    for (;;) {
        if (Serial.available() > 0) {
            String input = Serial.readStringUntil('\n');
            input.trim();

            double value; // Temporary variable for parsed values

            if (input == "RESTORE_DEFAULTS") {
                Serial.println("Resetting to default settings...");
                configManager.writeDefaultSettings();
                Serial.println("System reset to defaults. Please restart the device.");
                delay(1000); // Wait a moment to ensure the message is sent before rebooting
                esp_restart();
            // Print settings in serial monitor
            } else if (input.startsWith("PRINT_SETTINGS")) {
                printFileContents();
            // Reboot device
            } else if (input.startsWith("REBOOT")) {
                Serial.println("Rebooting...");
                delay(1000); // Wait a moment to ensure the message is sent before rebooting
                esp_restart();
            }
            // Handle unknown input
            else {
                Serial.println("Command not recognized. Please check your message and try again.\nPLEASE NOTE YOU ARE CONNECTED TO THE SLAVE ESP32\nCONNECT TO THE MASTER ESP32 TO UPDATE SETTINGS");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(500)); // Check every 500 ms
    }
}