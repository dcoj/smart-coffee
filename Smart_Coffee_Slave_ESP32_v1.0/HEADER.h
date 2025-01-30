#include <Wire.h>
#include <Adafruit_MAX31865.h>
#include "esp_system.h"
#include <AutoPID.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "FS.h"
#include <LittleFS.h>
#include <stdbool.h>
#include <driver/uart.h>
#include <string.h>

// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================