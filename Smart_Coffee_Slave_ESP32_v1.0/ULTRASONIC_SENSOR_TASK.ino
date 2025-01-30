#include "HEADER.h"

// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================

void ULTRASONIC_SENSOR_TASK(void *pvParameters) {
    for (;;) {
        if (ENTER_SAFE_MODE) {
            vTaskSuspend(NULL);
        }

        // Trigger the sensor
        digitalWrite(ULTRA_TRIGGER_PIN, LOW);
        delayMicroseconds(2);
        digitalWrite(ULTRA_TRIGGER_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(ULTRA_TRIGGER_PIN, LOW);

        // Wait for the pulse to complete
        while (!pulseComplete) {
            vTaskDelay(pdMS_TO_TICKS(1)); // Yield to other tasks
        }
        pulseComplete = false;

        long duration = pulseDuration;

        // Print the raw PWM duration
        //Serial.print("PWM Duration (us): ");
        //Serial.println(duration);

        if (duration > 0) { // Valid echo received
            long distance = duration / 5.75; // Convert to distance in mm

            // Print the calculated distance
            //Serial.print("Distance (mm): ");
            //Serial.println(distance);

            // Update tank percentage based on distance
            long tankPercentage = 100 - ((distance - configManager.config.RES_FULL) * 100 / (configManager.config.RES_EMPTY - configManager.config.RES_FULL));
            tankPercentage = constrain(tankPercentage, 0, 100);

            // Update measurement in the circular buffer
            measurements[US_INDEX] = tankPercentage;
            US_INDEX = (US_INDEX + 1) % MEASUREMENTS_COUNT; // Move to the next index, wrapping around if necessary

            // Calculate running average
            long sum = 0;
            for (int i = 0; i < MEASUREMENTS_COUNT; i++) {
                sum += measurements[i];
            }
            long average = sum / MEASUREMENTS_COUNT;

            // Print the running average
            //Serial.print("Running Average: ");
            //Serial.println(average);

            // Update the global variable with the mutex
            if (xSemaphoreTake(xMutexWaterResPercentage, portMAX_DELAY) == pdTRUE) {
                ULTRA_WATER_PERCENTAGE = average;
                xSemaphoreGive(xMutexWaterResPercentage);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(200)); // Delay before the next measurement
    }
}