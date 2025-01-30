#include "HEADER.h"
#include "Bitmap_icons.h"

// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================
// Below is the core functions of Smart-Coffee.
// To edit the default values for Smart-Coffee, click on the tab "CONFIG.h" above.


// ========== SEMAPHORES ========== 
SemaphoreHandle_t xMutexPressure;           // Mutex for kPa pressure value
SemaphoreHandle_t xMutexPressure2;          // Mutex for kPa pressure 2 value
SemaphoreHandle_t xMutexReservoirRefill;    // Mutex for "RESERVOIR_REFILL_REQUIRED" flag
SemaphoreHandle_t xMutexWeight;             // Mutex for load cell measured weight
SemaphoreHandle_t xMutexCurrentError;       // Mutex for checking the current error state
SemaphoreHandle_t xMutexTemprature1;        // Mutex for Temp 1
SemaphoreHandle_t xMutexTemprature2;        // Mutex for Temp 2
SemaphoreHandle_t xMutexDispensedWater;     // Mutex for flow meter dispensed water
SemaphoreHandle_t xMutexWaterResPercentage; // Mutex for ultrasonic water percentage value

// ========== TASK HANDLES ========== 
TaskHandle_t FlowMeterTaskHandle = NULL;
TaskHandle_t CheckErrorSafeHandle = NULL;
TaskHandle_t ReadLoadCellHandle = NULL;
TaskHandle_t BoilerWaterLevelCheckHandle = NULL;
TaskHandle_t StandbyModeHandle = NULL;
TaskHandle_t PWMB1ElementTaskHandle = NULL;
TaskHandle_t PIDB1ElementTaskHandle = NULL;
TaskHandle_t SerialOutputTaskHandle = NULL;
TaskHandle_t ReadPressureTaskHandle = NULL;
TaskHandle_t BrewCoffeeTaskHandle = NULL;
TaskHandle_t OneBoilerDisplayTaskHandle = NULL;
TaskHandle_t TwoBoilerDisplayTaskHandle = NULL;
TaskHandle_t UartTaskHandle = NULL;
TaskHandle_t CheckSerialInputHandle = NULL;
TaskHandle_t SwitchRelayTaskHandle = NULL;

// =========== UART SETUP ==================
#define TXD_PIN (GPIO_NUM_17)   // GPIO pin for TX
#define RXD_PIN (GPIO_NUM_16)   // GPIO pin for RX
#define UART_TX_BUF_SIZE 2048   // TX buffer size
#define UART_RX_BUF_SIZE 2048   // RX buffer size
#define HEARTBEAT_SEND_INTERVAL_MS 2000   // Send heartbeat every 2 seconds
#define HEARTBEAT_RECV_TIMEOUT_MS 6000    // Timeout for receiving heartbeat that results in an error message
static uint8_t uartBuffer1[UART_RX_BUF_SIZE];
static uint8_t uartBuffer2[UART_RX_BUF_SIZE];
static uint8_t* activeBuffer = uartBuffer1;
static uint8_t* processingBuffer = uartBuffer2;
static bool bufferFlag = true; // Flag to toggle between buffers

volatile bool REBOOT = false; // Flag to indicate the UART task to send a reboot request to the slave

void SW_BUFFERS() {
    if (bufferFlag) {
        activeBuffer = uartBuffer2;
        processingBuffer = uartBuffer1;
    } else {
        activeBuffer = uartBuffer1;
        processingBuffer = uartBuffer2;
    }
    bufferFlag = !bufferFlag;
}

void INITIALIZE_UART() {
    uart_config_t uart_config = {
        .baud_rate = 115200,      // Adjust as necessary
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_1, UART_TX_BUF_SIZE, UART_RX_BUF_SIZE, 0, NULL, 0);
}

// =========== GLOBAL VARIABLES ============
double PRESSURE_KPA_1;  // Global variable for calculated pressure
double PRESSURE_KPA_2; // pressure 2 from slave esp boiler
double TEMP_C_1 = 0.0; // Global variable for the temp 1
double TEMP_C_2 = 0.0; // Global variable for the temp 2

#define CONFIG_FILE_PATH "/config/settings.json"
struct Config {
    double B1_KPA;
    double B1_TEMP;
    double B1_KP;
    double B1_KI;
    double B1_KD;
    double SHOT_ML;
    double SHOT_GRAM;
    double WEIGHT_OFFSET;
    double SCALE_FACTOR;
    double FM1_HZ;
    double FM2_HZ;
    unsigned long SHOT_TIMER;
    unsigned long PRIME_PUMP;
    unsigned long INFUSION_HZ;
    unsigned long INFUSION_TIMER;
    double PT1_MIN_V;
    double PT1_MAX_V;
    double PT1_KPA;
    unsigned long B1_PWR;
    unsigned long B1_MAX_OT;
    unsigned long B1_MIN_OT;
    double B1_PWM;
    unsigned long B1_STABLE_PWM;
    double B1_STABLE_TH;
    double B1_MIN_KPA;
    double B1_MAX_KPA;
    unsigned long B1_REFILL_TIMER;
    unsigned long B2_REFILL_TIMER;
    unsigned long RES_REFILL_TIMER;
    bool SERIAL_PRINTING;
    bool ULTRASONIC_SENSOR;
    bool USE_IMPERIAL_UNITS;
    bool ESPRESSO_VOLUME;
    bool ESPRESSO_WEIGHT;
    bool ESPRESSO_TIMER;
    bool FLOW_METER;
    bool FLOW_METER_2;
    bool LOAD_CELL;
    bool DUAL_BOILER;
    bool STARTUP_LOGO;
    bool TEMP_SENSOR_1;
    bool TEMP_SENSOR_2;
    bool TRANSDUCER_1;
    bool TRANSDUCER_2;
    bool TRANSDUCER_1_KPA_SAFE;
    bool PRE_INFUSION;
    bool TARGET_TEMP_B1;
    bool PID_CONTROL_B1;
    bool PAUSE_REFILL;
    bool FLOW_METER_2_CAL;
    double B1_REFILL_TRIG;
    double B2_REFILL_TRIG;
    bool TOGGLE_BREW_SW;
    bool TOGGLE_ES_SW;
    bool B1_WTR_PROBE;
    bool B2_WTR_PROBE;
    bool DISABLE_RELAYS;

    // VALUES STORED FOR SLAVE ESP32
    double B2_KPA;
    double B2_TEMP;
    double B2_KP;
    double B2_KI;
    double B2_KD;
    double RES_PROBE;
    double RES_FULL;
    double RES_EMPTY;
    double RES_LOW;
    unsigned long B2_PWR;
    unsigned long B2_MAX_OT;
    unsigned long B2_MIN_OT;
    double B2_PWM;
    unsigned long B2_STABLE_PWM;
    double B2_STABLE_TH;
    double PT2_MIN_V;
    double PT2_MAX_V;
    double PT2_KPA;
    double B2_MIN_KPA;
    double B2_MAX_KPA;
    double TEMP_1_MIN;
    double TEMP_1_MAX;
    double TEMP_2_MIN;
    double TEMP_2_MAX;
    bool TRANSDUCER_2_KPA_SAFE;
    bool TEMP_1_SAFE;
    bool TEMP_2_SAFE;
    bool TARGET_TEMP_B2;
    bool PID_CONTROL_B2;
    bool RES_SW;
    bool RES_SW_NC;
    bool WTR_RES_PROBE;
    bool US_LVL_DETECT;

    Config() : B1_KPA(DEFAULT_B1_KPA), B1_TEMP(DEFAULT_B1_TEMP), B1_KP(DEFAULT_B1_KP), B1_KI(DEFAULT_B1_KI), B1_KD(DEFAULT_B1_KD),
               SHOT_ML(DEFAULT_SHOT_ML), SHOT_GRAM(DEFAULT_SHOT_GRAM), WEIGHT_OFFSET(DEFAULT_WEIGHT_OFFSET),
               SCALE_FACTOR(DEFAULT_SCALE_FACTOR), FM1_HZ(DEFAULT_FM1_HZ), FM2_HZ(DEFAULT_FM2_HZ),
               SHOT_TIMER(DEFAULT_SHOT_TIMER), PRIME_PUMP(DEFAULT_PRIME_PUMP),
               INFUSION_HZ(DEFAULT_INFUSION_HZ), INFUSION_TIMER(DEFAULT_INFUSION_TIMER), PT1_MIN_V(DEFAULT_PT1_MIN_V),
               PT1_MAX_V(DEFAULT_PT1_MAX_V), PT1_KPA(DEFAULT_PT1_KPA), B1_PWR(DEFAULT_B1_PWR), B1_MAX_OT(DEFAULT_B1_MAX_OT),
               B1_MIN_OT(DEFAULT_B1_MIN_OT), B1_PWM(DEFAULT_B1_PWM), B1_STABLE_PWM(DEFAULT_B1_STABLE_PWM), 
               B1_STABLE_TH(DEFAULT_B1_STABLE_TH), B1_MIN_KPA(DEFAULT_B1_MIN_KPA), B1_MAX_KPA(DEFAULT_B1_MAX_KPA),
               B1_REFILL_TIMER(DEFAULT_B1_REFILL_TIMER), B2_REFILL_TIMER(DEFAULT_B2_REFILL_TIMER), RES_REFILL_TIMER(DEFAULT_RES_REFILL_TIMER), SERIAL_PRINTING(DEFAULT_SERIAL_PRINTING),
               ULTRASONIC_SENSOR(DEFAULT_ULTRASONIC_SENSOR), USE_IMPERIAL_UNITS(DEFAULT_USE_IMPERIAL_UNITS), ESPRESSO_VOLUME(DEFAULT_ESPRESSO_VOLUME),
               ESPRESSO_WEIGHT(DEFAULT_ESPRESSO_WEIGHT), ESPRESSO_TIMER(DEFAULT_ESPRESSO_TIMER), FLOW_METER(DEFAULT_FLOW_METER), FLOW_METER_2(DEFAULT_FLOW_METER_2),
               LOAD_CELL(DEFAULT_LOAD_CELL), DUAL_BOILER(DEFAULT_DUAL_BOILER), STARTUP_LOGO(DEFAULT_STARTUP_LOGO), TEMP_SENSOR_1(DEFAULT_TEMP_SENSOR_1),
               TEMP_SENSOR_2(DEFAULT_TEMP_SENSOR_2), TRANSDUCER_1(DEFAULT_TRANSDUCER_1), TRANSDUCER_2(DEFAULT_TRANSDUCER_2), 
               TRANSDUCER_1_KPA_SAFE(DEFAULT_TRANSDUCER_1_KPA_SAFE), PRE_INFUSION(DEFAULT_PRE_INFUSION), TARGET_TEMP_B1(DEFAULT_TARGET_TEMP_B1),
               PID_CONTROL_B1(DEFAULT_PID_CONTROL_B1), PAUSE_REFILL(DEFAULT_PAUSE_REFILL), FLOW_METER_2_CAL(DEFAULT_FLOW_METER_2_CAL),
               B1_REFILL_TRIG(DEFAULT_B1_REFILL_TRIG), B2_REFILL_TRIG(DEFAULT_B2_REFILL_TRIG), TOGGLE_BREW_SW(DEFAULT_TOGGLE_BREW_SW), TOGGLE_ES_SW(DEFAULT_TOGGLE_ES_SW),
               B1_WTR_PROBE(DEFAULT_B1_WTR_PROBE), B2_WTR_PROBE(DEFAULT_B2_WTR_PROBE), DISABLE_RELAYS(DEFAULT_DISABLE_RELAYS),
               // VALUES STORED FOR SLAVE ESP32
               B2_KPA(DEFAULT_B2_KPA), B2_TEMP(DEFAULT_B2_TEMP), B2_KP(DEFAULT_B2_KP), B2_KI(DEFAULT_B2_KI), B2_KD(DEFAULT_B2_KD),
               RES_PROBE(DEFAULT_RES_PROBE), RES_FULL(DEFAULT_RES_FULL), RES_EMPTY(DEFAULT_RES_EMPTY), RES_LOW(DEFAULT_RES_LOW),
               B2_PWR(DEFAULT_B2_PWR), B2_MAX_OT(DEFAULT_B2_MAX_OT), B2_MIN_OT(DEFAULT_B2_MIN_OT), B2_PWM(DEFAULT_B2_PWM), B2_STABLE_PWM(DEFAULT_B2_STABLE_PWM), 
               B2_STABLE_TH(DEFAULT_B2_STABLE_TH), PT2_MIN_V(DEFAULT_PT2_MIN_V), PT2_MAX_V(DEFAULT_PT2_MAX_V), PT2_KPA(DEFAULT_PT2_KPA),
               B2_MIN_KPA(DEFAULT_B2_MIN_KPA), B2_MAX_KPA(DEFAULT_B2_MAX_KPA), TEMP_1_MIN(DEFAULT_TEMP_1_MIN), TEMP_1_MAX(DEFAULT_TEMP_1_MAX),
               TEMP_2_MIN(DEFAULT_TEMP_2_MIN), TEMP_2_MAX(DEFAULT_TEMP_2_MAX), TRANSDUCER_2_KPA_SAFE(DEFAULT_TRANSDUCER_2_KPA_SAFE),
               TEMP_1_SAFE(DEFAULT_TEMP_1_SAFE), TEMP_2_SAFE(DEFAULT_TEMP_2_SAFE), TARGET_TEMP_B2(DEFAULT_TARGET_TEMP_B2), PID_CONTROL_B2(DEFAULT_PID_CONTROL_B2),
               RES_SW(DEFAULT_RES_SW), RES_SW_NC(DEFAULT_RES_SW_NC), WTR_RES_PROBE(DEFAULT_WTR_RES_PROBE), US_LVL_DETECT(DEFAULT_US_LVL_DETECT) {}
};

class ConfigurationManager {
public:
    Config config;

    ConfigurationManager() {
        loadSettings();
    }

    void loadSettings() {
        fs::File file = LittleFS.open(CONFIG_FILE_PATH, FILE_READ);
        if (!file) {
            Serial.println("Config file not found, creating with default settings.");
            file.close();
            writeDefaultSettings(); // Create the file with default settings
            return;
        }

        StaticJsonDocument<2048> doc;
        DeserializationError error = deserializeJson(doc, file);
        if (error) {
            Serial.println("Failed to read config file, creating with default settings.");
            return;
        }

        // Load each setting from the file, or use default if not found
        config.B1_KPA = doc["B1_KPA"] | DEFAULT_B1_KPA;
        config.B1_TEMP = doc["B1_TEMP"] | DEFAULT_B1_TEMP;
        config.B1_KP = doc["B1_KP"] | DEFAULT_B1_KP;
        config.B1_KI = doc["B1_KI"] | DEFAULT_B1_KI;
        config.B1_KD = doc["B1_KD"] | DEFAULT_B1_KD;
        config.SHOT_ML = doc["SHOT_ML"] | DEFAULT_SHOT_ML;
        config.SHOT_GRAM = doc["SHOT_GRAM"] | DEFAULT_SHOT_GRAM;
        config.WEIGHT_OFFSET = doc["WEIGHT_OFFSET"] | DEFAULT_WEIGHT_OFFSET;
        config.SCALE_FACTOR = doc["SCALE_FACTOR"] | DEFAULT_SCALE_FACTOR;
        config.FM1_HZ = doc["FM1_HZ"] | DEFAULT_FM1_HZ;
        config.FM2_HZ = doc["FM2_HZ"] | DEFAULT_FM2_HZ;
        config.SHOT_TIMER = doc["SHOT_TIMER"] | DEFAULT_SHOT_TIMER;
        config.PRIME_PUMP = doc["PRIME_PUMP"] | DEFAULT_PRIME_PUMP;
        config.INFUSION_HZ = doc["INFUSION_HZ"] | DEFAULT_INFUSION_HZ;
        config.INFUSION_TIMER = doc["INFUSION_TIMER"] | DEFAULT_INFUSION_TIMER;
        config.PT1_MIN_V = doc["PT1_MIN_V"] | DEFAULT_PT1_MIN_V;
        config.PT1_MAX_V = doc["PT1_MAX_V"] | DEFAULT_PT1_MAX_V;
        config.PT1_KPA = doc["PT1_KPA"] | DEFAULT_PT1_KPA;
        config.B1_PWR = doc["B1_PWR"] | DEFAULT_B1_PWR;
        config.B1_MAX_OT = doc["B1_MAX_OT"] | DEFAULT_B1_MAX_OT;
        config.B1_MIN_OT = doc["B1_MIN_OT"] | DEFAULT_B1_MIN_OT;
        config.B1_PWM = doc["B1_PWM"] | DEFAULT_B1_PWM;
        config.B1_STABLE_PWM = doc["B1_STABLE_PWM"] | DEFAULT_B1_STABLE_PWM;
        config.B1_STABLE_TH = doc["B1_STABLE_TH"] | DEFAULT_B1_STABLE_TH;
        config.B1_MIN_KPA = doc["B1_MIN_KPA"] | DEFAULT_B1_MIN_KPA;
        config.B1_MAX_KPA = doc["B1_MAX_KPA"] | DEFAULT_B1_MAX_KPA;
        config.B1_REFILL_TIMER = doc["B1_REFILL_TIMER"] | DEFAULT_B1_REFILL_TIMER;
        config.B2_REFILL_TIMER = doc["B2_REFILL_TIMER"] | DEFAULT_B2_REFILL_TIMER;
        config.RES_REFILL_TIMER = doc["RES_REFILL_TIMER"] | DEFAULT_RES_REFILL_TIMER;
        config.SERIAL_PRINTING = doc["SERIAL_PRINTING"] | DEFAULT_SERIAL_PRINTING;
        config.ULTRASONIC_SENSOR = doc["ULTRASONIC_SENSOR"] | DEFAULT_ULTRASONIC_SENSOR;
        config.USE_IMPERIAL_UNITS = doc["USE_IMPERIAL_UNITS"] | DEFAULT_USE_IMPERIAL_UNITS;
        config.ESPRESSO_VOLUME = doc["ESPRESSO_VOLUME"] | DEFAULT_ESPRESSO_VOLUME;
        config.ESPRESSO_WEIGHT = doc["ESPRESSO_WEIGHT"] | DEFAULT_ESPRESSO_WEIGHT;
        config.ESPRESSO_TIMER = doc["ESPRESSO_TIMER"] | DEFAULT_ESPRESSO_TIMER;
        config.FLOW_METER = doc["FLOW_METER"] | DEFAULT_FLOW_METER;
        config.FLOW_METER_2 = doc["FLOW_METER_2"] | DEFAULT_FLOW_METER_2;
        config.LOAD_CELL = doc["LOAD_CELL"] | DEFAULT_LOAD_CELL;
        config.DUAL_BOILER = doc["DUAL_BOILER"] | DEFAULT_DUAL_BOILER;
        config.STARTUP_LOGO = doc["STARTUP_LOGO"] | DEFAULT_STARTUP_LOGO;
        config.TEMP_SENSOR_1 = doc["TEMP_SENSOR_1"] | DEFAULT_TEMP_SENSOR_1;
        config.TEMP_SENSOR_2 = doc["TEMP_SENSOR_2"] | DEFAULT_TEMP_SENSOR_2;
        config.TRANSDUCER_1 = doc["TRANSDUCER_1"] | DEFAULT_TRANSDUCER_1;
        config.TRANSDUCER_2 = doc["TRANSDUCER_2"] | DEFAULT_TRANSDUCER_2;
        config.TRANSDUCER_1_KPA_SAFE = doc["TRANSDUCER_1_KPA_SAFE"] | DEFAULT_TRANSDUCER_1_KPA_SAFE;
        config.PRE_INFUSION = doc["PRE_INFUSION"] | DEFAULT_PRE_INFUSION;
        config.TARGET_TEMP_B1 = doc["TARGET_TEMP_B1"] | DEFAULT_TARGET_TEMP_B1;
        config.PID_CONTROL_B1 = doc["PID_CONTROL_B1"] | DEFAULT_PID_CONTROL_B1;
        config.PAUSE_REFILL = doc["PAUSE_REFILL"] | DEFAULT_PAUSE_REFILL;
        config.FLOW_METER_2_CAL = doc["FLOW_METER_2_CAL"] | DEFAULT_FLOW_METER_2_CAL;
        config.B1_REFILL_TRIG = doc["B1_REFILL_TRIG"] | DEFAULT_B1_REFILL_TRIG;
        config.B2_REFILL_TRIG = doc["B2_REFILL_TRIG"] | DEFAULT_B2_REFILL_TRIG;
        config.TOGGLE_BREW_SW = doc["TOGGLE_BREW_SW"] | DEFAULT_TOGGLE_BREW_SW;
        config.TOGGLE_ES_SW = doc["TOGGLE_ES_SW"] | DEFAULT_TOGGLE_ES_SW;
        config.B1_WTR_PROBE = doc["B1_WTR_PROBE"] | DEFAULT_B1_WTR_PROBE;
        config.B2_WTR_PROBE = doc["B2_WTR_PROBE"] | DEFAULT_B2_WTR_PROBE;
        config.DISABLE_RELAYS = doc["DISABLE_RELAYS"] | DEFAULT_DISABLE_RELAYS;

        // VALUES STORED FOR SLAVE ESP32
        config.B2_KPA = doc["B2_KPA"] | DEFAULT_B2_KPA;
        config.B2_TEMP = doc["B2_TEMP"] | DEFAULT_B2_TEMP;
        config.B2_KP = doc["B2_KP"] | DEFAULT_B2_KP;
        config.B2_KI = doc["B2_KI"] | DEFAULT_B2_KI;
        config.B2_KD = doc["B2_KD"] | DEFAULT_B2_KD;
        config.RES_PROBE = doc["RES_PROBE"] | DEFAULT_RES_PROBE;
        config.RES_FULL = doc["RES_FULL"] | DEFAULT_RES_FULL;
        config.RES_EMPTY = doc["RES_EMPTY"] | DEFAULT_RES_EMPTY;
        config.RES_LOW = doc["RES_LOW"] | DEFAULT_RES_LOW;
        config.B2_PWR = doc["B2_PWR"] | DEFAULT_B2_PWR;
        config.B2_MAX_OT = doc["B2_MAX_OT"] | DEFAULT_B2_MAX_OT;
        config.B2_MIN_OT = doc["B2_MIN_OT"] | DEFAULT_B2_MIN_OT;
        config.B2_PWM = doc["B2_PWM"] | DEFAULT_B2_PWM;
        config.B2_STABLE_PWM = doc["B2_STABLE_PWM"] | DEFAULT_B2_STABLE_PWM;
        config.B2_STABLE_TH = doc["B2_STABLE_TH"] | DEFAULT_B2_STABLE_TH;
        config.PT2_MIN_V = doc["PT2_MIN_V"] | DEFAULT_PT2_MIN_V;
        config.PT2_MAX_V = doc["PT2_MAX_V"] | DEFAULT_PT2_MAX_V;
        config.PT2_KPA = doc["PT2_KPA"] | DEFAULT_PT2_KPA;
        config.B2_MIN_KPA = doc["B2_MIN_KPA"] | DEFAULT_B2_MIN_KPA;
        config.B2_MAX_KPA = doc["B2_MAX_KPA"] | DEFAULT_B2_MAX_KPA;
        config.TEMP_1_MIN = doc["TEMP_1_MIN"] | DEFAULT_TEMP_1_MIN;
        config.TEMP_1_MAX = doc["TEMP_1_MAX"] | DEFAULT_TEMP_1_MAX;
        config.TEMP_2_MIN = doc["TEMP_2_MIN"] | DEFAULT_TEMP_2_MIN;
        config.TEMP_2_MAX = doc["TEMP_2_MAX"] | DEFAULT_TEMP_2_MAX;
        config.TRANSDUCER_2_KPA_SAFE = doc["TRANSDUCER_2_KPA_SAFE"] | DEFAULT_TRANSDUCER_2_KPA_SAFE;
        config.TEMP_1_SAFE = doc["TEMP_1_SAFE"] | DEFAULT_TEMP_1_SAFE;
        config.TEMP_2_SAFE = doc["TEMP_2_SAFE"] | DEFAULT_TEMP_2_SAFE;
        config.TARGET_TEMP_B2 = doc["TARGET_TEMP_B2"] | DEFAULT_TARGET_TEMP_B2;
        config.PID_CONTROL_B2 = doc["PID_CONTROL_B2"] | DEFAULT_PID_CONTROL_B2;
        config.RES_SW = doc["RES_SW"] | DEFAULT_RES_SW;
        config.RES_SW_NC = doc["RES_SW_NC"] | DEFAULT_RES_SW_NC;
        config.WTR_RES_PROBE = doc["WTR_RES_PROBE"] | DEFAULT_WTR_RES_PROBE;
        config.US_LVL_DETECT = doc["US_LVL_DETECT"] | DEFAULT_US_LVL_DETECT;
        file.close();
    }

    void saveSettings() {
        // Check if the /config directory exists, create it if it doesn't
        if (!LittleFS.exists("/config")) {
            if (!LittleFS.mkdir("/config")) {
                Serial.println("Failed to create /config directory.");
                return; // Exit if unable to create the directory
            }
        }

        // Set each setting to the document
        StaticJsonDocument<2048> doc;
        doc["B1_KPA"] = config.B1_KPA;
        doc["B1_TEMP"] = config.B1_TEMP;
        doc["B1_KP"] = config.B1_KP;
        doc["B1_KI"] = config.B1_KI;
        doc["B1_KD"] = config.B1_KD;
        doc["SHOT_ML"] = config.SHOT_ML;
        doc["SHOT_GRAM"] = config.SHOT_GRAM;
        doc["WEIGHT_OFFSET"] = config.WEIGHT_OFFSET;
        doc["SCALE_FACTOR"] = config.SCALE_FACTOR;
        doc["FM1_HZ"] = config.FM1_HZ;
        doc["FM2_HZ"] = config.FM2_HZ;
        doc["SHOT_TIMER"] = config.SHOT_TIMER;
        doc["PRIME_PUMP"] = config.PRIME_PUMP;
        doc["INFUSION_HZ"] = config.INFUSION_HZ;
        doc["INFUSION_TIMER"] = config.INFUSION_TIMER;
        doc["PT1_MIN_V"] = config.PT1_MIN_V;
        doc["PT1_MAX_V"] = config.PT1_MAX_V;
        doc["PT1_KPA"] = config.PT1_KPA;
        doc["B1_PWR"] = config.B1_PWR;
        doc["B1_MAX_OT"] = config.B1_MAX_OT;
        doc["B1_MIN_OT"] = config.B1_MIN_OT;
        doc["B1_PWM"] = config.B1_PWM;
        doc["B1_STABLE_PWM"] = config.B1_STABLE_PWM;
        doc["B1_STABLE_TH"] = config.B1_STABLE_TH;
        doc["B1_MIN_KPA"] = config.B1_MIN_KPA;
        doc["B1_MAX_KPA"] = config.B1_MAX_KPA;
        doc["B1_REFILL_TIMER"] = config.B1_REFILL_TIMER;
        doc["B2_REFILL_TIMER"] = config.B2_REFILL_TIMER;
        doc["RES_REFILL_TIMER"] = config.RES_REFILL_TIMER;
        doc["SERIAL_PRINTING"] = config.SERIAL_PRINTING;
        doc["ULTRASONIC_SENSOR"] = config.ULTRASONIC_SENSOR;
        doc["USE_IMPERIAL_UNITS"] = config.USE_IMPERIAL_UNITS;
        doc["ESPRESSO_VOLUME"] = config.ESPRESSO_VOLUME;
        doc["ESPRESSO_WEIGHT"] = config.ESPRESSO_WEIGHT;
        doc["ESPRESSO_TIMER"] = config.ESPRESSO_TIMER;
        doc["FLOW_METER"] = config.FLOW_METER;
        doc["FLOW_METER_2"] = config.FLOW_METER_2;
        doc["LOAD_CELL"] = config.LOAD_CELL;
        doc["DUAL_BOILER"] = config.DUAL_BOILER;
        doc["STARTUP_LOGO"] = config.STARTUP_LOGO;
        doc["TEMP_SENSOR_1"] = config.TEMP_SENSOR_1;
        doc["TEMP_SENSOR_2"] = config.TEMP_SENSOR_2;
        doc["TRANSDUCER_1"] = config.TRANSDUCER_1;
        doc["TRANSDUCER_2"] = config.TRANSDUCER_2;
        doc["TRANSDUCER_1_KPA_SAFE"] = config.TRANSDUCER_1_KPA_SAFE;
        doc["PRE_INFUSION"] = config.PRE_INFUSION;
        doc["TARGET_TEMP_B1"] = config.TARGET_TEMP_B1;
        doc["PID_CONTROL_B1"] = config.PID_CONTROL_B1;
        doc["PAUSE_REFILL"] = config.PAUSE_REFILL;
        doc["FLOW_METER_2_CAL"] = config.FLOW_METER_2_CAL;
        doc["B1_REFILL_TRIG"] = config.B1_REFILL_TRIG;
        doc["B2_REFILL_TRIG"] = config.B2_REFILL_TRIG;
        doc["TOGGLE_BREW_SW"] = config.TOGGLE_BREW_SW;
        doc["TOGGLE_ES_SW"] = config.TOGGLE_ES_SW;
        doc["B1_WTR_PROBE"] = config.B1_WTR_PROBE;
        doc["B2_WTR_PROBE"] = config.B2_WTR_PROBE;
        doc["DISABLE_RELAYS"] = config.DISABLE_RELAYS;

        // VALUES STORED FOR SLAVE ESP32
        doc["B2_KPA"] = config.B2_KPA;
        doc["B2_TEMP"] = config.B2_TEMP;
        doc["B2_KP"] = config.B2_KP;
        doc["B2_KI"] = config.B2_KI;
        doc["B2_KD"] = config.B2_KD;
        doc["RES_PROBE"] = config.RES_PROBE;
        doc["RES_FULL"] = config.RES_FULL;
        doc["RES_EMPTY"] = config.RES_EMPTY;
        doc["RES_LOW"] = config.RES_LOW;
        doc["B2_PWR"] = config.B2_PWR;
        doc["B2_MAX_OT"] = config.B2_MAX_OT;
        doc["B2_MIN_OT"] = config.B2_MIN_OT;
        doc["B2_PWM"] = config.B2_PWM;
        doc["B2_STABLE_PWM"] = config.B2_STABLE_PWM;
        doc["B2_STABLE_TH"] = config.B2_STABLE_TH;
        doc["PT2_MIN_V"] = config.PT2_MIN_V;
        doc["PT2_MAX_V"] = config.PT2_MAX_V;
        doc["PT2_KPA"] = config.PT2_KPA;
        doc["B2_MIN_KPA"] = config.B2_MIN_KPA;
        doc["B2_MAX_KPA"] = config.B2_MAX_KPA;
        doc["TEMP_1_MIN"] = config.TEMP_1_MIN;
        doc["TEMP_1_MAX"] = config.TEMP_1_MAX;
        doc["TEMP_2_MIN"] = config.TEMP_2_MIN;
        doc["TEMP_2_MAX"] = config.TEMP_2_MAX;
        doc["TRANSDUCER_2_KPA_SAFE"] = config.TRANSDUCER_2_KPA_SAFE;
        doc["TEMP_1_SAFE"] = config.TEMP_1_SAFE;
        doc["TEMP_2_SAFE"] = config.TEMP_2_SAFE;
        doc["TARGET_TEMP_B2"] = config.TARGET_TEMP_B2;
        doc["PID_CONTROL_B2"] = config.PID_CONTROL_B2;
        doc["RES_SW"] = config.RES_SW;
        doc["RES_SW_NC"] = config.RES_SW_NC;
        doc["WTR_RES_PROBE"] = config.WTR_RES_PROBE;
        doc["US_LVL_DETECT"] = config.US_LVL_DETECT;

        fs::File file = LittleFS.open(CONFIG_FILE_PATH, FILE_WRITE);
        if (!file) {
            Serial.println("Failed to open config file for writing.");
            return;
        }

        if (serializeJson(doc, file) == 0) {
            Serial.println("Failed to write to file.");
        }
        file.close();
    }

    void writeDefaultSettings() {
        // Reset all settings to their default values
        config = Config();
        saveSettings();
    }
};


void RESTORE_LFS_DEFAULTS() {
    if (!LittleFS.begin()) {
        Serial.println("LittleFS Mount Failed. Formatting LittleFS...");
        if (LittleFS.format()) {
            Serial.println("LittleFS format successful.");
            if (!LittleFS.begin()) { // Try to mount again after formatting
                Serial.println("LittleFS mount failed after formatting.");
                return;
            }
        } else {
            Serial.println("LittleFS format failed.");
            return;
        }
    }
    Serial.println("LittleFS mounted successfully.");
}

ConfigurationManager configManager; // Define configManager globally

void PRINT_SETTINGS_M() {
    // Open the file for reading
    fs::File file = LittleFS.open(CONFIG_FILE_PATH, FILE_READ);
    if (!file) {
        Serial.println("Failed to open file for reading.");
        return;
    }

    // Parse the JSON from the file
    StaticJsonDocument<2048> doc; // Ensure this size is adequate for your JSON structure
    DeserializationError error = deserializeJson(doc, file);
    if (error) {
        Serial.println("Failed to read file, using default configuration");
    } else {
        // Iterate over each key-value pair and print
        for (JsonPair kv : doc.as<JsonObject>()) {
            Serial.print(kv.key().c_str()); // Print the key
            Serial.print(": ");

            // Check the type of the value and print accordingly
            if (kv.value().is<const char*>()) { // If the value is a string
                Serial.println(kv.value().as<const char*>());
            } else if (kv.value().is<long>()) { // If the value is an integer
                Serial.println(kv.value().as<long>(), DEC);
            } else if (kv.value().is<double>()) { // If the value is a double
                Serial.println(kv.value().as<double>(), 6); // Adjust precision as needed
            } else {
                // For other types, you might want to convert them to string
                Serial.println(kv.value().as<String>());
            }
        }
    }

    // Close the file
    file.close();
    
}

// SEND SHARED VALUES TO SLAVE ESP32
void SEND_BOOT_SETUP() {
    StaticJsonDocument<2048> doc;
    doc["B2_REFILL_TRIG"] = configManager.config.B2_REFILL_TRIG;
    doc["SERIAL_PRINTING"] = configManager.config.SERIAL_PRINTING;
    doc["ULTRASONIC_SENSOR"] = configManager.config.ULTRASONIC_SENSOR;
    doc["DUAL_BOILER"] = configManager.config.DUAL_BOILER;
    doc["TEMP_SENSOR_1"] = configManager.config.TEMP_SENSOR_1;
    doc["TEMP_SENSOR_2"] = configManager.config.TEMP_SENSOR_2;
    doc["TRANSDUCER_2"] = configManager.config.TRANSDUCER_2;
    doc["B2_KPA"] = configManager.config.B2_KPA;
    doc["B2_TEMP"] = configManager.config.B2_TEMP;
    doc["B2_KP"] = configManager.config.B2_KP;
    doc["B2_KI"] = configManager.config.B2_KI;
    doc["B2_KD"] = configManager.config.B2_KD;
    doc["RES_PROBE"] = configManager.config.RES_PROBE;
    doc["RES_FULL"] = configManager.config.RES_FULL;
    doc["RES_EMPTY"] = configManager.config.RES_EMPTY;
    doc["RES_LOW"] = configManager.config.RES_LOW;
    doc["B2_PWR"] = configManager.config.B2_PWR;
    doc["B2_MAX_OT"] = configManager.config.B2_MAX_OT;
    doc["B2_MIN_OT"] = configManager.config.B2_MIN_OT;
    doc["B2_PWM"] = configManager.config.B2_PWM;
    doc["B2_STABLE_PWM"] = configManager.config.B2_STABLE_PWM;
    doc["B2_STABLE_TH"] = configManager.config.B2_STABLE_TH;
    doc["PT2_MIN_V"] = configManager.config.PT2_MIN_V;
    doc["PT2_MAX_V"] = configManager.config.PT2_MAX_V;
    doc["PT2_KPA"] = configManager.config.PT2_KPA;
    doc["B2_MIN_KPA"] = configManager.config.B2_MIN_KPA;
    doc["B2_MAX_KPA"] = configManager.config.B2_MAX_KPA;
    doc["TEMP_1_MIN"] = configManager.config.TEMP_1_MIN;
    doc["TEMP_1_MAX"] = configManager.config.TEMP_1_MAX;
    doc["TEMP_2_MIN"] = configManager.config.TEMP_2_MIN;
    doc["TEMP_2_MAX"] = configManager.config.TEMP_2_MAX;
    doc["TRANSDUCER_2_KPA_SAFE"] = configManager.config.TRANSDUCER_2_KPA_SAFE;
    doc["TEMP_1_SAFE"] = configManager.config.TEMP_1_SAFE;
    doc["TEMP_2_SAFE"] = configManager.config.TEMP_2_SAFE;
    doc["TARGET_TEMP_B2"] = configManager.config.TARGET_TEMP_B2;
    doc["PID_CONTROL_B2"] = configManager.config.PID_CONTROL_B2;
    doc["RES_SW"] = configManager.config.RES_SW;
    doc["RES_SW_NC"] = configManager.config.RES_SW_NC;
    doc["WTR_RES_PROBE"] = configManager.config.WTR_RES_PROBE;
    doc["US_LVL_DETECT"] = configManager.config.US_LVL_DETECT;
    doc["B1_WTR_PROBE"] = configManager.config.B1_WTR_PROBE;
    doc["B2_WTR_PROBE"] = configManager.config.B2_WTR_PROBE;


    char buffer[2048];
    serializeJson(doc, buffer);
    uart_write_bytes(UART_NUM_1, buffer, strlen(buffer));
    uart_write_bytes(UART_NUM_1, "\n", 1);
}

volatile bool SEND_PT2_SETTINGS = false;

void sendPT2Settings() {
    StaticJsonDocument<512> doc;
    doc["B2_KPA"] = configManager.config.B2_KPA;
    doc["PT2_MAX_V"] = configManager.config.PT2_MAX_V;
    doc["PT2_MIN_V"] = configManager.config.PT2_MIN_V;
    doc["PT2_KPA"] = configManager.config.PT2_KPA;

    char buffer[512];
    serializeJson(doc, buffer);
    uart_write_bytes(UART_NUM_1, buffer, strlen(buffer));
    uart_write_bytes(UART_NUM_1, "\n", 1);
}

// Stop tasks safely to update/change settings
void STOP_TASKS() {
    if (FlowMeterTaskHandle != NULL) {
        vTaskDelete(FlowMeterTaskHandle);
    }
    if (CheckErrorSafeHandle != NULL) {
        vTaskDelete(CheckErrorSafeHandle);
    }
    if (ReadLoadCellHandle != NULL) {
        vTaskDelete(ReadLoadCellHandle);
    }
    if (BoilerWaterLevelCheckHandle != NULL) {
        vTaskDelete(BoilerWaterLevelCheckHandle);
    }
    if (PWMB1ElementTaskHandle != NULL) {
        vTaskDelete(PWMB1ElementTaskHandle);
    }
    if (PIDB1ElementTaskHandle != NULL) {
        vTaskDelete(PIDB1ElementTaskHandle);
    }
    if (SerialOutputTaskHandle != NULL) {
        vTaskDelete(SerialOutputTaskHandle);
    }
    if (ReadPressureTaskHandle != NULL) {
        vTaskDelete(ReadPressureTaskHandle);
    }
    if (BrewCoffeeTaskHandle != NULL) {
        vTaskDelete(BrewCoffeeTaskHandle);
    }
    if (OneBoilerDisplayTaskHandle != NULL) {
        vTaskDelete(OneBoilerDisplayTaskHandle);
    }
    if (TwoBoilerDisplayTaskHandle != NULL) {
        vTaskDelete(TwoBoilerDisplayTaskHandle);
    }
    if (StandbyModeHandle != NULL) {
        vTaskDelete(StandbyModeHandle);
    }
    if (SwitchRelayTaskHandle != NULL) {
        vTaskDelete(SwitchRelayTaskHandle);
    }

}



// ========== FLOW METER ==========
// FLOW METER 1
#define FLOW_METER_1_PIN 25  // GPIO pin connected to the first flow meter
volatile unsigned long FM1_PULSE_COUNT = 0;  // Pulse count for the first flow meter
float DISPENSED_WATER = 0.0;  // Net dispensed water in millilitres
// Interrupt Service Routines
void IRAM_ATTR FM1_PULSE_COUNTER() {
    FM1_PULSE_COUNT++;
}

// FLOW METER 2
#define FLOW_METER_2_PIN 26  // GPIO pin connected to the second flow meter
volatile unsigned long FM2_PULSE_COUNT = 0;  // Pulse count for the second flow meter
void IRAM_ATTR FM2_PULSE_COUNTER() {
    FM2_PULSE_COUNT++;
}


// ========== SAFE-MODE ==========
enum ErrorCode {
    NO_ERROR,
    TRANSDUCER_1_SM,
    TRANSDUCER_2_SM,
    TEMP_SENSOR_1_SM,
    TEMP_SENSOR_2_SM,
    // Commnication
    LOST_UART_CONNECTION,
};
ErrorCode currentError = NO_ERROR;
volatile bool ENTER_SAFE_MODE = false; // Global flag for error state

// ========== CALIBRATION ==========
volatile bool CALIBRATION_MODE = false; // Global flag to indicate if calibration mode is ON
volatile bool CALIBRATION_MODE_ICON = false;

// ========== RESERVOIR WATER LEVEL PROBE ==========
volatile bool RESERVOIR_REFILL_REQUIRED = false; // Global flag to disable relays and display "Refill Water Reservoir Message"

// ========== BOILER WATER LEVEL ==========
volatile int B1_TOUCH_VALUE = 0;
volatile int B2_TOUCH_VALUE = 0;
volatile bool REFILL_IN_PROGRESS = false;  // Flag to indicate if a boiler refill is in progress
volatile bool REFILL_B1 = false; //Global flag to indicate Boiler 1 refilling
volatile bool ELEMENT_2_STATE = false; //Global flag to indicate if Element 2 is On or Off


// ========== LOAD CELL ==========
HX711 scale;
float calibration_factor;
#define LOAD_CELL_DOUT_PIN 25
#define LOAD_CELL_SCK_PIN 26
float LOAD_CELL_WEIGHT = 0; // Measured weight in grams
volatile bool CALIBRATING_LOAD_CELL = false; // Global flag for calibrating loadcell


// ========== ULTRASONIC SENSOR ==========
volatile int ULTRA_WATER_PERCENTAGE = 100; // Global variable for water percentage

// ========== RELAY and SWITCHES ==========
#define PUMP_PIN 27     // Pump relay pin
#define RLY1_PIN 14     // Relay 1 pin (normally connected to boiler 1 refill solenoid)
#define RLY2_PIN 19     // Relay 2 pin (normally connected to boiler 2 refill solenoid)
#define RLY3_PIN 13     // Relay 3 pin
#define RLY4_PIN 33     // Relay 4 pin
#define BREW_SW_PIN 36  // Brew switch/lever pin
#define ES_MODE_PIN 39  // Espresso mode switch pin

// ========== TOGGLE SWITCHES ==========
volatile bool ES_SW_ON = false;
volatile bool BREW_SW_ON = false;

volatile bool BREW_SW_PRESSED = false;  // Current state of the brew switch
volatile bool ES_SW_PRESSED = false;  // Current state of the espresso mode switch

volatile bool BREW_SW_DEBOUNCE = false;  // Debounced state of the brew switch
volatile bool ES_SW_DEBOUNCE = false;  // Debounced state of the espresso mode switch

void IRAM_ATTR handleBrewSwitch() {
    static uint32_t last_interrupt_time = 0;
    uint32_t interrupt_time = millis();
    if (interrupt_time - last_interrupt_time > 5) { // debounce time in ms
        BREW_SW_PRESSED = digitalRead(BREW_SW_PIN) == LOW;
    }
    last_interrupt_time = interrupt_time;
}

void IRAM_ATTR handleEsModeSwitch() {
    static uint32_t last_interrupt_time = 0;
    uint32_t interrupt_time = millis();
    if (interrupt_time - last_interrupt_time > 5) { // debounce time in ms
        ES_SW_PRESSED = digitalRead(ES_MODE_PIN) == LOW;
    }
    last_interrupt_time = interrupt_time;
}

// ========== PRE-INFUSION ==========
enum ShotModeState {
    SHOT_IDLE,           // Wait state, ready for next shot
    SHOT_PRIME,          // Priming state
    SHOT_PRE_INFUSION,   // Pre-infusion state
    SHOT_BREWING,        // Brewing state, until espresso shot target/timer is reached
    SHOT_FINISHED        // Shot finished
};
ShotModeState currentShotState = SHOT_IDLE;
unsigned long shotStartTime = 0;
int preInfusionCycleCount = 0;

const unsigned long PRE_INFUSION_CYCLE_TIME = (1000 / (2 * configManager.config.INFUSION_HZ));

enum PreInfusionState {
    PRE_INFUSION_ON,
    PRE_INFUSION_OFF
};
PreInfusionState preInfusionState = PRE_INFUSION_OFF;

// ========== RESERVOIR PROBE TOUCH VALUE FOR CALIBTAION ==========
volatile float RES_TOUCH_VALUE = 0.0;

// ========== PRESSURE TRANSDUCER 2 ==========
volatile float PT2_VOLTAGE = 0.0;
volatile bool CALIBRATING_B2 = false; // Flag to indicate if PT1 is being calibrated


// ========== PRESSURE TRANSDUCER 1 ==========
const int PRESSURE_SENSOR_PIN = 35;     // Pressure transducer sensor pin
volatile bool CALIBRATING_B1 = false; // Flag to indicate if PT1 is being calibrated

// Buffer to hold the last 5 voltage readings for running average
float voltageReadings[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
int readingIndex = 0;

#define VOLTAGE_DIVIDER_RATIO 1.51 // Voltage devider ratio used to lower the transducer signal

// =============== PID ELEMENT CONTROL ===============
AutoPIDRelay* myAutoPIDRelay = nullptr;  // Pointer for AutoPIDRelay
double pulseWidth = 1000;
bool relayState = false;
double* E1_TARGET;
double* E1_INPUT;

// =============== PWM ELEMENT CONTROL ===============
double ELEMENT;
#define ELEMENT_1 32 // Boiler heating element pin

// ========== OLED DISPLAY ==========
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128
#define OLED_RESET -1
#define SDA_PIN 21
#define SCL_PIN 22
Adafruit_SH1107 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, 200000);

volatile bool SHOW_CAL_ICON = false; // Flag to flash the calibration icon

// ========== WATER RESERVOIR DISPLAY BAR ==========
#define RECT_WIDTH 15 // Width of the water level rectangle
#define RECT_HEIGHT 90 // Height of the rectangle
#define RECT_X 110 // X-positon starting from the left
#define RECT_Y 0 // Y-position of the rectangle, starting at the top



// =============== TASKS ===============

// UART MASTER TASK
extern void UART_TASK(void *pvParameters);

// FLOW METER TASK
extern void FLOW_METER_TASK(void *pvParameters);

// SAFE-MODE TASK
extern void SAFE_MODE_TASK(void *pvParameters);

// BOILER WATER LEVEL CHECK TASK
extern void BOILER_WTR_TASK(void *pvParameters);

// RESERVOIR WATER LEVEL CHECK TASK
extern void STANDBY_MODE(void *pvParameters);

// BREWING COFFEE
extern void BREW_COFFEE_TASK(void *pvParameters);

// READING PRESSURE
extern void READ_PRESSURE_TASK(void *pvParameters);

// SERIAL INPUT
extern void SERIAL_INPUT_TASK(void *pvParameters);

// LOAD CELL
extern void LOAD_CELL_TASK(void *pvParameters);

// LOAD CELL CALIBRATION
extern void LOAD_CELL_CALIBRATION(void *pvParameters);

// SWITCHES AND RELAYS TASK
extern void SWITCHES_RELAYS_TASK(void *pvParameters);

// SINGLE BOILER DISPLAY
extern void SINGLE_BOILER_DISPLAY_TASK(void *pvParameters);

// DUAL BOILER DISPLAY
extern void DUAL_BOILER_DISPLAY_TASK(void *pvParameters);

// CONTROL ELEMENT
extern void PWM_B1_ELEMENT_TASK(void *pvParameters);
extern void PID_B1_ELEMENT_TASK(void *pvParameters);


void setup() {
    Serial.begin(115200);
    // DISPLAY STARTUP
    display.begin(0x3C);
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    display.display();

    delay(1000); // Wait for serial monitor to open
    RESTORE_LFS_DEFAULTS(); // Overwrite settings with defaults values if file did not load
    delay(500); // Wait for format
    configManager = ConfigurationManager(); // Ensures LittleFS is checked and formatted if necessary

    INITIALIZE_UART();// Start UART

    // CONFIG PRESSURE OR TEMP TARGET FOR PID/PWM ELEMENT CONTROL
    if (configManager.config.TARGET_TEMP_B1) {
        E1_TARGET = &configManager.config.B1_TEMP;
        E1_INPUT = &TEMP_C_1;
    } else {
        E1_TARGET = &configManager.config.B1_KPA;
        E1_INPUT = &PRESSURE_KPA_1;
    }

    // Dynamically create the AutoPIDRelay instance
    myAutoPIDRelay = new AutoPIDRelay(E1_INPUT, E1_TARGET, &relayState, pulseWidth, configManager.config.B1_KP, configManager.config.B1_KI, configManager.config.B1_KD);

    // INPUT SWITCHES
    if (configManager.config.TOGGLE_BREW_SW) {
        pinMode(BREW_SW_PIN, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(BREW_SW_PIN), handleBrewSwitch, CHANGE);
    } else {
        pinMode(BREW_SW_PIN, INPUT_PULLUP);
    }

    if (configManager.config.TOGGLE_ES_SW) {
        pinMode(ES_MODE_PIN, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(ES_MODE_PIN), handleEsModeSwitch, CHANGE);
    } else {
        pinMode(ES_MODE_PIN, INPUT_PULLUP);
    }

    // BOILER 1 ELEMENT RELAY
    pinMode(ELEMENT_1, OUTPUT);
    digitalWrite(ELEMENT_1, HIGH);


    // WATER PUMP RELAY PIN
    pinMode(PUMP_PIN, OUTPUT);
    digitalWrite(PUMP_PIN, HIGH);

    // RELAY 1
    pinMode(RLY1_PIN, OUTPUT);
    digitalWrite(RLY1_PIN, HIGH);

    // RELAY 2
    pinMode(RLY2_PIN, OUTPUT);
    digitalWrite(RLY2_PIN, HIGH);

    // RELAY 3
    pinMode(RLY3_PIN, OUTPUT);
    digitalWrite(RLY3_PIN, HIGH);

    // RELAY 4
    pinMode(RLY4_PIN, OUTPUT);
    digitalWrite(RLY4_PIN, HIGH);

    // If DISABLE_RELAYS is true, turn off boiler 1 and 2
    if (configManager.config.DISABLE_RELAYS) {
        CALIBRATING_B1 = true;
        CALIBRATING_B2 = true;
    }

    // FLOW METER 1
    if (configManager.config.FLOW_METER) {
        pinMode(FLOW_METER_1_PIN, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(FLOW_METER_1_PIN), FM1_PULSE_COUNTER, RISING);
        xMutexDispensedWater = xSemaphoreCreateMutex(); // Flow meter dispensed water
    }
    // FLOW METER 2
    if (configManager.config.FLOW_METER_2) {
        pinMode(FLOW_METER_2_PIN, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(FLOW_METER_2_PIN), FM2_PULSE_COUNTER, RISING);
    }

    // LOAD CELL
    if (configManager.config.LOAD_CELL) {
        scale.begin(LOAD_CELL_DOUT_PIN, LOAD_CELL_SCK_PIN);
        if (configManager.config.SCALE_FACTOR != DEFAULT_SCALE_FACTOR) {
            scale.set_scale(configManager.config.SCALE_FACTOR);
        } else {
            scale.set_scale(DEFAULT_SCALE_FACTOR);
        }
        scale.tare();
        xMutexWeight = xSemaphoreCreateMutex(); // Load scale measured weight
    }



    // CREATE MUTEX
    xMutexTemprature1  = xSemaphoreCreateMutex(); // Temp 1
    xMutexTemprature2  = xSemaphoreCreateMutex(); // Temp 2
    xMutexCurrentError = xSemaphoreCreateMutex(); // Safe-mode current error state
    xMutexReservoirRefill = xSemaphoreCreateMutex(); // Reservoir refill required bool
    xMutexWaterResPercentage = xSemaphoreCreateMutex(); // Ultrasonic sensor water percentage
    xMutexPressure = xSemaphoreCreateMutex(); // Pressure reading
    xMutexPressure2 = xSemaphoreCreateMutex(); // Pressure 2 reading


    // CREATE TASKS
    // Run on core 0
    xTaskCreatePinnedToCore(
        UART_TASK,      // Task function
        "UART Task",         // Name of the task (for debugging)
        4096,                      // Stack size in words
        NULL,                      // Task input parameter
        5,                         // Priority of the task
        &UartTaskHandle,            // Task handle
        0                          // Core where the task should run
    );


    // Run on core 1

    if (configManager.config.DUAL_BOILER) {
        xTaskCreatePinnedToCore(
            DUAL_BOILER_DISPLAY_TASK,      // Task function
            "Dual Boiler Display Task",    // Name of the task (for debugging)
            2048,                          // Stack size in words
            NULL,                          // Task input parameter
            1,                             // Priority of the task
            &TwoBoilerDisplayTaskHandle,   // Task handle
            0                              // Core where the task should run
        ); 
    } else {
        xTaskCreatePinnedToCore(
            SINGLE_BOILER_DISPLAY_TASK,     // Task function
            "Single Boiler Display Task",   // Name of the task (for debugging)
            2048,                           // Stack size in words
            NULL,                           // Task input parameter
            1,                              // Priority of the task
            &OneBoilerDisplayTaskHandle,    // Task handle
            0                               // Core where the task should run
        ); 
 
    } 

    xTaskCreatePinnedToCore(
        BREW_COFFEE_TASK,             // Task function
        "Brew Coffee",                // Name of the task (for debugging)
        4096,                         // Stack size in words
        NULL,                         // Task input parameter
        1,                            // Priority of the task
        &BrewCoffeeTaskHandle,        // Task handle
        1                             // Core where the task should run
    );  

    xTaskCreatePinnedToCore(
        SWITCHES_RELAYS_TASK,             // Task function
        "Switches and relay task",                // Name of the task (for debugging)
        1028,                         // Stack size in words
        NULL,                         // Task input parameter
        1,                            // Priority of the task
        &SwitchRelayTaskHandle,        // Task handle
        1                             // Core where the task should run
    ); 

    if (configManager.config.TRANSDUCER_1) {
        xTaskCreatePinnedToCore(
            READ_PRESSURE_TASK,       // Task function
            "Read Pressure",          // Name of the task (for debugging)
            2048,                     // Stack size in words
            NULL,                     // Task input parameter
            1,                        // Priority of the task
            &ReadPressureTaskHandle,  // Task handle
            1                         // Core where the task should run
        );
    }

    if (configManager.config.PID_CONTROL_B1) {
        xTaskCreatePinnedToCore(
            PID_B1_ELEMENT_TASK,       // Task function
            "PID B1 element task",     // Name of the task (for debugging)
            2048,                      // Stack size in words
            NULL,                      // Task input parameter
            1,                         // Priority of the task
            &PIDB1ElementTaskHandle,   // Task handle
            1                          // Core where the task should run
        );  
    } else {
        xTaskCreatePinnedToCore(
            PWM_B1_ELEMENT_TASK,      // Task function
            "PWM B1 element task",    // Name of the task (for debugging)
            2048,                     // Stack size in words
            NULL,                     // Task input parameter
            1,                        // Priority of the task
            &PWMB1ElementTaskHandle,  // Task handle
            1                         // Core where the task should run
        );  
    }

    xTaskCreatePinnedToCore(
        BOILER_WTR_TASK,     // Task function
        "BoilerWaterLevelCheck",      // Name of the task (for debugging)
        2048,                         // Stack size in words
        NULL,                         // Task input parameter
        2,                            // Priority of the task
        &BoilerWaterLevelCheckHandle, // Task handle
        1                             // Core where the task should run
    );  

    xTaskCreatePinnedToCore(
        STANDBY_MODE,                    // Task function
        "StandbyMode",   // Name of the task (for debugging)
        2048,                         // Stack size in words
        NULL,                         // Task input parameter
        3,                            // Priority of the task
        &StandbyModeHandle, // Task handle
        1                             // Core where the task should run
    ); 

    xTaskCreatePinnedToCore(
        SERIAL_INPUT_TASK,          // Task function
        "Serial Input Task",        // Name of the task (for debugging)
        4096,                       // Stack size in words
        NULL,                       // Task input parameter
        1,                          // Priority of the task
        &CheckSerialInputHandle,    // Task handle
        1                           // Core where the task should run
    );  



    if (configManager.config.LOAD_CELL) {
        xTaskCreatePinnedToCore(
            LOAD_CELL_TASK,         // Task function
            "Load Cell Task",       // Name of the task (for debugging)
            2048,                   // Stack size in words
            NULL,                   // Task input parameter
            1,                      // Priority of the task
            &ReadLoadCellHandle,    // Task handle
            1                       // Core where the task should run
        );
    }

    xTaskCreatePinnedToCore(
        SAFE_MODE_TASK,             // Task function
        "Check and Safe Mode",      // Name of the task (for debugging)
        2048,                       // Stack size in words
        NULL,                       // Task input parameter
        5,                          // Priority of the task
        &CheckErrorSafeHandle,      // Task handle
        1                           // Core where the task should run
    ); 

    if (configManager.config.FLOW_METER) {
        xTaskCreatePinnedToCore(
            FLOW_METER_TASK,          // Task function
            "Flow Meter Task",        // Name of the task (for debugging)
            2048,                     // Stack size in words
            NULL,                     // Task input parameter
            1,                        // Priority of the task
            &FlowMeterTaskHandle,     // Task handle
            1                         // Core where the task should run
        );
    }
}


void loop() {
// Loop left empty for RTOS program
}
