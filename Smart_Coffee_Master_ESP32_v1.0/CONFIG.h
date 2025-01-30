// ============================================
// ============= DISCLAIMER ===================
// ============================================
// Smart Coffee code ("Software") is provided "as is" without warranty of any kind, either express or implied,
// including but not limited to the implied warranties of merchantability and fitness for a particular purpose.
// Consumer Responsibility:
// By using this Software, you acknowledge that the code is intended for educational and experimental purposes only.
// The Software is not intended to control espresso machines or any other equipment in a way that meets
// any safety, regulatory, or quality standards. If you choose to customize or tune the code to operate your
// espresso machine, you do so at your own risk and are fully responsible for any damage, harm, or
// inconsistencies that may result, including but not limited to damage to your espresso machine, harm to
// individuals, or subpar quality of the espresso produced.

// By using this Software, you agree to indemnify, defend, and hold harmless the creator, contributors, and
// distributors from any and all claims, liabilities, damages, and/or costs (including but not limited to attorney
// fees) arising from your use or misuse of the Software, your violation of this disclaimer, or your violation of any
// rights of a third party.

// Limitation of Liability:
// The creator, contributors, and distributors of this Software shall in no event be held liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages (including, but not limited to, procurement
// of substitute goods or services; loss of use, data, or profits; or business interruption) however caused and on
// any theory of liability, whether in contract, strict liability, or tort (including negligence or otherwise) arising in
// any way out of the use of this Software, even if advised of the possibility of such damage.

// In order to use the code, you must agree to the disclaimer above by writing true below
#define I_AGREE_TO_THE_TERMS_IN_THE_DISCLAIMER true



// ==================================================
// ============= DEFAULT FEATURES ===================
// ==================================================
// INFO:
// false = feature is disabled by default
// true = feature is enabled by default

// ============= CORE FEATURES =============
#define DEFAULT_DUAL_BOILER false         // Enabled/Disable single or dual boiler mode

#define DEFAULT_PAUSE_REFILL true        // Enabled/Disable pauses a boiler refill if the machine is in the process of making an espresso shot

#define DEFAULT_USE_IMPERIAL_UNITS false  // Enabled/Disable display in imperial units

#define DEFAULT_STARTUP_LOGO true         // Enabled/Disable the smart coffee startup logo

#define DEFAULT_SERIAL_PRINTING true     // Serial printing of several sensor values

#define DEFAULT_DISABLE_RELAYS false      // Enable/Disable relays (prevents relays from turning on)

// ============= PRE-INFUSION SETTINGS =============
#define DEFAULT_PRE_INFUSION false        // Enabled/Disable Pre-Infusion (Works when espresso mode is active only)
#define DEFAULT_PRIME_PUMP 1000           // Pre-infusion stage 1: Prime pump (value in milliseconds. 1000ms = 1 second)
#define DEFAULT_INFUSION_HZ 5             // Pre-infusion switching frequency in Hz (cycles per second)
#define DEFAULT_INFUSION_TIMER 5000       // Pre-infusion stage 2 timer:  Cycle the pump on/off (value in milliseconds. 1000ms = 1 second)

// =============BREW AND ESPRESSO SWITCH LOGIC =============
#define DEFAULT_TOGGLE_BREW_SW false      // False = Pump ON when the switch is closed. True = Pump will toggle on/off with each press of the switch
#define DEFAULT_TOGGLE_ES_SW false        // False = Espresso mode ON when the switch is closed. True = espresso mode will toggle on/off with each press of the switch
#define DEFAULT_TOGGLE_STEAM_SW false     // False = Steam mode ON when switch is closed. True = Steam mode will toggle on/off with each press

// ============= PRESSURE TRANSDUCER 1 =============
#define DEFAULT_TRANSDUCER_1 true           // Enabled/Disable pressure transducer 1
// Sensor signal settings
#define DEFAULT_PT1_MIN_V 0.5               // Pressure transducer 1: Minimum voltage from pressure sensor at 0 kPa
#define DEFAULT_PT1_MAX_V 4.5               // Pressure transducer 1: Maximum voltage from pressure sensor at max pressure rating
#define DEFAULT_PT1_KPA 200.0               // Pressure transducer 1: Peak pressure rating in kPa
// Safety minimum and maximum limits
#define DEFAULT_TRANSDUCER_1_KPA_SAFE true  // Enable/Disable min and max kPa pressure values for pressure transducer 1
#define DEFAULT_B1_MIN_KPA -15              // Boiler 1 minimum pressure for safe-mode if enabled
#define DEFAULT_B1_MAX_KPA 120              // Boiler 1 maximum pressure for safe-mode if enabled
// Pressure or Temprature target
#define DEFAULT_TARGET_TEMP_B1 true        // Enabled/Disable target temperature for boiler 1 (disabled = target is pressure in kPa)
#define DEFAULT_B1_KPA 50                   // Boiler 1 target kPa value
#define DEFAULT_B1_STEAM_KPA 50             // Boiler 1 target kPa value
#define DEFAULT_B1_TEMP 93                  // Boiler 1 target Celsius value
#define DEFAULT_B1_STEAM_TEMP 140           // Steam mode temperature target in Celsius

// PWM Tuning
// (These settings are only used if "DEFAULT_PID_CONTROL_B1" is false)
#define DEFAULT_B1_PWR 200                  // Boiler 1 PWR duration (value in milliseconds. 1000ms = 1 second)
#define DEFAULT_B1_MAX_OT 800               // Boiler 1 max off time (value in milliseconds. 1000ms = 1 second)
#define DEFAULT_B1_MIN_OT 100               // Boiler 1 min off time (value in milliseconds. 1000ms = 1 second)
#define DEFAULT_B1_PWM 5.0                  // Boiler 1 PWM threshold
#define DEFAULT_B1_STABLE_PWM 2000          // Boiler 1 stable PWM (value in milliseconds. 1000ms = 1 second)
#define DEFAULT_B1_STABLE_TH 2              // Boiler 2 stable threshold
//PID tuning
// (These settings are only used if "DEFAULT_PID_CONTROL_B1" is true)
#define DEFAULT_PID_CONTROL_B1 false  // Enabled/Disable PID logic for boiler 1 (disabled = PWM logic is used for boiler 1)
#define DEFAULT_B1_KP 1.0             // Boiler 1 Kp value
#define DEFAULT_B1_KI 0.1             // Boiler 1 Ki value
#define DEFAULT_B1_KD 0.1             // Boiler 1 Kd value

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
#define DEFAULT_TEMP_1_MIN 10                // Temp sensor 1 minimum temperature for safe-mode if enabled
#define DEFAULT_TEMP_1_MAX 180              // Temp sensor 1 maximum temperature for safe-mode if enabled

// ============= TEMPERATURE SENSOR 2 =============
#define DEFAULT_TEMP_SENSOR_2 true        // Enable/Disable temp sensor 2
#define DEFAULT_TEMP_2_SAFE false          // Enable/Disable min and max temp Celsius values for Safe-Mode
#define DEFAULT_TEMP_2_MIN 10               // Temp sensor 2 minimum temperature for safe-mode if enabled
#define DEFAULT_TEMP_2_MAX 120             // Temp sensor 2 maximum temperature for safe-mode if enabled

// ============= ESPRESSO SHOT TARGET (enable only 1 of the 3 options) =============
#define DEFAULT_ESPRESSO_VOLUME false      // Enabled/Disable espresso shot target by volume (use with flow meters)
#define DEFAULT_ESPRESSO_WEIGHT false      // Enabled/Disable espresso shot target by weight (use with load cell)
#define DEFAULT_ESPRESSO_TIMER true       // Enabled/Disable espresso shot timer

// ============= ESPRESSO SHOT TIMER =============
#define DEFAULT_SHOT_TIMER 16000           // Espresso shot timer (value in milliseconds. 1000ms = 1 second)

// ============= FLOW METER =============
#define DEFAULT_FLOW_METER false          // Enabled/Disable the Flow meter
#define DEFAULT_FLOW_METER_2 false        // Enabled/Disable the 2nd flow meter
#define DEFAULT_FLOW_METER_2_CAL false    // Enabled/Disable calibration mode of the 2nd flow meter
#define DEFAULT_SHOT_ML 40.0              // Espresso shot volume target in millilitres (when using flow meters)
#define DEFAULT_FM1_HZ 4400               // pulses per litre for flow meter 1
#define DEFAULT_FM2_HZ 120000             // pulses per litre for flow meter 2

// ============= LOAD CELL =============
#define DEFAULT_LOAD_CELL false           // Enabled/Disable the load cell
#define DEFAULT_SHOT_GRAM 36             // Espresso shot gram target (when using loadcell)
#define DEFAULT_WEIGHT_OFFSET 5          // Weight offset to switch the pump off before hitting the gram target
#define DEFAULT_SCALE_FACTOR 1000        // Load cell scale factor value (Use the calibrate load cell function to set this value)


// ============= BOILER 1 PROBE AND REFILL TIMER =============
#define DEFAULT_B1_WTR_PROBE false        // Enable/Disable the water level detection probe to trigger a refill for boiler 1
#define DEFAULT_B1_REFILL_TRIG 5         // Boiler 1 water level probe trigger threshold
#define DEFAULT_B1_REFILL_TIMER 2000     // Boiler 1 refill timer (value in milliseconds. 1000ms = 1 second)

// ============= BOILER 2 PROBE AND REFILL TIMER =============
#define DEFAULT_B2_WTR_PROBE false       // Enable/Disable the water level detection probe to trigger a refill for boiler 2
#define DEFAULT_B2_REFILL_TRIG 5         // Boiler 2 water level probe trigger threshold
#define DEFAULT_B2_REFILL_TIMER 2000     // Boiler 2 refill timer (value in milliseconds. 1000ms = 1 second)

// ============= RESERVOIR REFILL TIMER =============
#define DEFAULT_RES_REFILL_TIMER 0      // Reservoir refill timer (value in milliseconds. 1000ms = 1 second)


// =============  WATER RESERVOIR LEVEL DETECTION =============
// Enable only 1 out of the 3 options water level detection options: Reservoir switch, Reservoir touch sensitive probe, or the ultrasonic sensor

// Reservoir switch
#define DEFAULT_RES_SW true              // When enabled, once the RES pin is pulled to GND, the machine will shut-down untill the reservoir is refilled
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


// =========================================
// ============= RELAY CONFIG ==============
// =========================================
// To enable a relay, uncomment it by removing the leading "//".
// To disable a relay, comment it out by writing "//" in front

// ========= BOILER 1 REFILL =========
#define PUMP_ON_B1_REFILL
//#define RELAY1_ON_B1_REFILL
//#define RELAY2_ON_B1_REFILL
//#define RELAY3_ON_B1_REFILL
//#define RELAY4_ON_B1_REFILL

// ========= BOILER 2 REFILL =========
//#define PUMP_ON_B2_REFILL
//#define RELAY1_ON_B2_REFILL
//#define RELAY2_ON_B2_REFILL
//#define RELAY3_ON_B2_REFILL
//#define RELAY4_ON_B2_REFILL

// ========= RESERVOIR REFILL =========
//#define PUMP_ON_RES_REFILL
//#define RELAY1_ON_RES_REFILL
//#define RELAY2_ON_RES_REFILL
//#define RELAY3_ON_RES_REFILL
//#define RELAY4_ON_RES_REFILL

// ========= DURING BREWING =========
#define PUMP_ON_DURING_BREW
//#define RELAY1_ON_DURING_BREW
//#define RELAY2_ON_DURING_BREW
//#define RELAY3_ON_DURING_BREW
//#define RELAY4_ON_DURING_BREW

// ========= END OF BREWING =========
//#define RELAY1_ON_END_BREW
//#define RELAY2_ON_END_BREW
//#define RELAY3_ON_END_BREW
//#define RELAY4_ON_END_BREW

