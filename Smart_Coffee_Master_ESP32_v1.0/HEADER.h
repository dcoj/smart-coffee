#include <Wire.h>
#include "esp_system.h"
#include "HX711.h"
#include <Adafruit_SH110X.h>
#include <AutoPID.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "FS.h"
#include <LittleFS.h>
#include <stdbool.h>
#include <driver/uart.h>
#include <string.h>
#include "CONFIG.h"
#include "COMPILE_TIME_CHECKS.h"

// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================