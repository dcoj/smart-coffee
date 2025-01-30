// ==================================================
// ============= DEFAULT FEATURES ===================
// ==================================================
// INFO:
// These values can be modified to match the settings in the Master program. 
// However, this is not necessary since the Slave receives the settings from the master on startup

// ============= CORE FEATURES =============

#define DEFAULT_SERIAL_PRINTING false      // Serial printing of several sensor values

#define DEFAULT_DUAL_BOILER false          // Enabled/Disable single or dual boiler mode

// ============= BOILER 1 PROBE AND REFILL TIMER =============
#define DEFAULT_B1_WTR_PROBE false         // Enable/Disable the water level detection probe to trigger a refill for boiler 1

// ============= BOILER 2 PROBE AND REFILL TIMER =============
#define DEFAULT_B2_WTR_PROBE false         // Enable/Disable the water level detection probe to trigger a refill for boiler 2
#define DEFAULT_B2_WTR_PROBE false         // Enable/Disable the water level detection probe to trigger a refill for boiler 2
#define DEFAULT_B2_REFILL_TRIG 5           // Boiler 2 water level probe trigger threshold

// ============= PRESSURE TRANSDUCER 2 =============
#define DEFAULT_TRANSDUCER_2 false           // Enabled/Disable pressure transducer 2
// Sensor signal settings
#define DEFAULT_PT2_MIN_V 0.5                // Pressure transducer 2: Minimum voltage from pressure sensor at 0 kPa
#define DEFAULT_PT2_MAX_V 4.5                // Pressure transducer 2: Maximum voltage from pressure sensor at max pressure rating
#define DEFAULT_PT2_KPA 200                  // Pressure transducer 2: Peak pressure rating in kPa
// Safety minimum and maximum limits
#define DEFAULT_TRANSDUCER_2_KPA_SAFE false  // Enable/Disable min and max kPa pressure values for pressure transducer 2
#define DEFAULT_B2_MIN_KPA -15               // Boiler 2 minimum pressure for safe-mode if enabled
#define DEFAULT_B2_MAX_KPA 50                // Boiler 2 maximum pressure for safe-mode if enabled
// Pressure or Temprature target
#define DEFAULT_TARGET_TEMP_B2 false         // Enabled/Disable target temperature for boiler 2 (disabled = target is pressure in kPa)
#define DEFAULT_B2_KPA 100                   // Boiler 2 target kPa value
#define DEFAULT_B2_TEMP 50                   // Boiler 2 target Celsius value
// PWM Tuning
// (These settings are only used if "DEFAULT_PID_CONTROL_B2" is false)
#define DEFAULT_B2_PWR 200                   // Boiler 2 PWR duration (value in milliseconds. 1000ms = 1 second)
#define DEFAULT_B2_MAX_OT 800                // Boiler 2 max off time (value in milliseconds. 1000ms = 1 second)
#define DEFAULT_B2_MIN_OT 100                // Boiler 2 min off time (value in milliseconds. 1000ms = 1 second)
#define DEFAULT_B2_PWM 5.0                   // Boiler 2 PWM threshold
#define DEFAULT_B2_STABLE_PWM 2000           // Boiler 2 stable PWM (value in milliseconds. 1000ms = 1 second)
#define DEFAULT_B2_STABLE_TH 2               // Boiler 2 stable threshold
//PID tuning
// (These settings are only used if "DEFAULT_PID_CONTROL_B2" is true)
#define DEFAULT_PID_CONTROL_B2 false         // Enabled/Disable PID logic for boiler 2 (disabled = PWM logic is used for boiler 2)
#define DEFAULT_B2_KP 1.0                    // Boiler 2 Kp value
#define DEFAULT_B2_KI 0.1                    // Boiler 2 Ki value
#define DEFAULT_B2_KD 0.1                    // Boiler 2 Kd value

// ============= TEMPERATURE SENSOR 1 =============
#define DEFAULT_TEMP_SENSOR_1 true          // Enable/Disable temp sensor 1
#define DEFAULT_TEMP_1_SAFE false           // Enable/Disable min and max temp Celsius values for Safe-Mode
#define DEFAULT_TEMP_1_MIN 2                // Temp sensor 1 minimum temperature for safe-mode if enabled
#define DEFAULT_TEMP_1_MAX 120              // Temp sensor 1 maximum temperature for safe-mode if enabled

// ============= TEMPERATURE SENSOR 2 =============
#define DEFAULT_TEMP_SENSOR_2 false        // Enable/Disable temp sensor 2
#define DEFAULT_TEMP_2_SAFE false          // Enable/Disable min and max temp Celsius values for Safe-Mode
#define DEFAULT_TEMP_2_MIN 2               // Temp sensor 2 minimum temperature for safe-mode if enabled
#define DEFAULT_TEMP_2_MAX 120             // Temp sensor 2 maximum temperature for safe-mode if enabled

// =============  WATER RESERVOIR LEVEL DETECTION ============= 
// Enable only 1 out of the 3 options water level detection options: Reservoir switch, Reservoir touch sensitive probe, or the ultrasonic sensor

// Reservoir switch
#define DEFAULT_RES_SW false              // When enabled, once the RES pin is pulled to GND, the machine will shut-down untill the reservoir is refilled
#define DEFAULT_RES_SW_NC false           // Inverts the RES_SW logic if the RES pin is noramlly connected to GND when the reservoir is full

// Reservoir touch sensitive probe
#define DEFAULT_WTR_RES_PROBE false       // When enabled, once the water drops below the probe, the machine will shut-down untill the reservoir is refilled
#define DEFAULT_RES_PROBE 70             // Water reservoir probe touch sensitive value

// Ultrasonic sensor
#define DEFAULT_ULTRASONIC_SENSOR false    // Enabled/Disable the ultrasonic sensor
#define DEFAULT_US_LVL_DETECT false       // When enabled and the water percentage falls below the "DEFAULT_RES_LOW" value, the machine will shut-down untill the reservoir is refilled
#define DEFAULT_RES_FULL 50               // Distance (in millimeters) from the face of the sensor, to the water level when full
#define DEFAULT_RES_EMPTY 250             // Distance (in millimeters) from the face of the sensor, to the water level when empty
#define DEFAULT_RES_LOW 10                // Minimum water remaining percentage before shutting down untill the reservoir is refilled

