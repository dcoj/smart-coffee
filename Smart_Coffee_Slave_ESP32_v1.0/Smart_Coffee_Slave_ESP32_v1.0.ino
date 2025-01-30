#include "HEADER.h"
#include "PROGRAM_DEFAULTS.h"


// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================
// Below is the core functions of Smart-Coffee.


// ========== RESERVOIR ==========
#define RESERVOIR_PIN T4 // Boiler water level probe/switch pin
volatile int RES_PROBE_TOUCH_VALUE = 0; // Global variable for the reservoir probe value
volatile bool RESERVOIR_REFILL_REQUIRED = false; // Global flag to disable relays and display "Refill Water Reservoir Message"
volatile bool RESERVOIR_REFILL_REQUIRED_UART = false; // Global flag to send UART message to master
volatile bool RESERVOIR_FULL_UART = false; // Global flag to send UART message to master
volatile int SP_RES_PROBE = 0; // Serial printing variable for the reservoir probe


// =========== UART SETUP ==================
#define TXD_PIN (GPIO_NUM_17)   // GPIO pin for TX
#define RXD_PIN (GPIO_NUM_16)   // GPIO pin for RX
#define UART_TX_BUF_SIZE 2048   // TX buffer size
#define UART_RX_BUF_SIZE 2048   // RX buffer size
static uint8_t uartBuffer1[UART_RX_BUF_SIZE];
static uint8_t uartBuffer2[UART_RX_BUF_SIZE];
static uint8_t* activeBuffer = uartBuffer1; // This buffer is currently being filled
static uint8_t* processingBuffer = uartBuffer2; // This buffer is for processing
static bool bufferFlag = true; // Flag to toggle between buffers

void switchBuffers() {
    if (bufferFlag) {
        activeBuffer = uartBuffer2;
        processingBuffer = uartBuffer1;
    } else {
        activeBuffer = uartBuffer1;
        processingBuffer = uartBuffer2;
    }
    bufferFlag = !bufferFlag;
}

#define HEARTBEAT_SEND_INTERVAL_MS 2000   // Send heartbeat every 2 seconds
#define HEARTBEAT_RECV_TIMEOUT_MS 6000    // Timeout for receiving heartbeat


// Macros for converting values to unsigned longs
#define DEFAULT_B2_PWR_UL (DEFAULT_B2_PWR * 1000UL)
#define DEFAULT_B2_MAX_OT_UL (DEFAULT_B2_MAX_OT * 1000UL)
#define DEFAULT_B2_MIN_OT_UL (DEFAULT_B2_MIN_OT * 1000UL)
#define DEFAULT_B2_STABLE_PWM_UL (DEFAULT_B2_STABLE_PWM * 1000UL)

#define CONFIG_FILE_PATH "/config/settings.json"

struct Config {
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
    bool SERIAL_PRINTING;
    bool ULTRASONIC_SENSOR;
    bool TEMP_SENSOR_1;
    bool TEMP_SENSOR_2;
    bool TRANSDUCER_2;
    bool TRANSDUCER_2_KPA_SAFE;
    bool TEMP_1_SAFE;
    bool TEMP_2_SAFE;
    bool DUAL_BOILER;
    bool TARGET_TEMP_B2;
    bool PID_CONTROL_B2;
    bool RES_SW;
    bool RES_SW_NC;
    bool WTR_RES_PROBE;
    bool US_LVL_DETECT;
    double B2_REFILL_TRIG;
    bool B1_WTR_PROBE;
    bool B2_WTR_PROBE;

    Config() : B2_KPA(DEFAULT_B2_KPA), B2_TEMP(DEFAULT_B2_TEMP),
               B2_KP(DEFAULT_B2_KP), B2_KI(DEFAULT_B2_KI), B2_KD(DEFAULT_B2_KD), RES_PROBE(DEFAULT_RES_PROBE), RES_FULL(DEFAULT_RES_FULL),
               RES_EMPTY(DEFAULT_RES_EMPTY), RES_LOW(DEFAULT_RES_LOW), B2_PWR(DEFAULT_B2_PWR_UL), B2_MAX_OT(DEFAULT_B2_MAX_OT_UL),
               B2_MIN_OT(DEFAULT_B2_MIN_OT_UL), B2_PWM(DEFAULT_B2_PWM), B2_STABLE_PWM(DEFAULT_B2_STABLE_PWM_UL), B2_STABLE_TH(DEFAULT_B2_STABLE_TH),
               PT2_MIN_V(DEFAULT_PT2_MIN_V), PT2_MAX_V(DEFAULT_PT2_MAX_V), PT2_KPA(DEFAULT_PT2_KPA), B2_MIN_KPA(DEFAULT_B2_MIN_KPA), B2_MAX_KPA(DEFAULT_B2_MAX_KPA),
               TEMP_1_MIN(DEFAULT_TEMP_1_MIN), TEMP_1_MAX(DEFAULT_TEMP_1_MAX), TEMP_2_MIN(DEFAULT_TEMP_2_MIN), TEMP_2_MAX(DEFAULT_TEMP_2_MAX),
               SERIAL_PRINTING(DEFAULT_SERIAL_PRINTING), ULTRASONIC_SENSOR(DEFAULT_ULTRASONIC_SENSOR),
               TEMP_SENSOR_1(DEFAULT_TEMP_SENSOR_1), TEMP_SENSOR_2(DEFAULT_TEMP_SENSOR_2), TRANSDUCER_2(DEFAULT_TRANSDUCER_2),
               TRANSDUCER_2_KPA_SAFE(DEFAULT_TRANSDUCER_2_KPA_SAFE), TEMP_1_SAFE(DEFAULT_TEMP_1_SAFE), TEMP_2_SAFE(DEFAULT_TEMP_2_SAFE), 
               DUAL_BOILER(DEFAULT_DUAL_BOILER), TARGET_TEMP_B2(DEFAULT_TARGET_TEMP_B2), PID_CONTROL_B2(DEFAULT_PID_CONTROL_B2), RES_SW(DEFAULT_RES_SW), 
               RES_SW_NC(DEFAULT_RES_SW_NC), WTR_RES_PROBE(DEFAULT_WTR_RES_PROBE), US_LVL_DETECT(DEFAULT_US_LVL_DETECT), B2_REFILL_TRIG(DEFAULT_B2_REFILL_TRIG),
               B1_WTR_PROBE(DEFAULT_B1_WTR_PROBE), B2_WTR_PROBE(DEFAULT_B2_WTR_PROBE) {}
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

        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, file);
        if (error) {
            Serial.println("Failed to read config file, creating with default settings.");
            return;
        }

        // Load each setting from the file, or use default if not found
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
        config.B2_MAX_OT = doc["B2_MAX_OT"] | DEFAULT_B2_MAX_OT_UL;
        config.B2_MIN_OT = doc["B2_MIN_OT"] | DEFAULT_B2_MIN_OT_UL;
        config.B2_PWM = doc["B2_PWM"] | DEFAULT_B2_PWM;
        config.B2_STABLE_PWM = doc["B2_STABLE_PWM"] | DEFAULT_B2_STABLE_PWM_UL;
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
        config.SERIAL_PRINTING = doc["SERIAL_PRINTING"] | DEFAULT_SERIAL_PRINTING;
        config.ULTRASONIC_SENSOR = doc["ULTRASONIC_SENSOR"] | DEFAULT_ULTRASONIC_SENSOR;
        config.TEMP_SENSOR_1 = doc["TEMP_SENSOR_1"] | DEFAULT_TEMP_SENSOR_1;
        config.TEMP_SENSOR_2 = doc["TEMP_SENSOR_2"] | DEFAULT_TEMP_SENSOR_2;
        config.TRANSDUCER_2 = doc["TRANSDUCER_2"] | DEFAULT_TRANSDUCER_2;
        config.TRANSDUCER_2_KPA_SAFE = doc["TRANSDUCER_2_KPA_SAFE"] | DEFAULT_TRANSDUCER_2_KPA_SAFE;
        config.TEMP_1_SAFE = doc["TEMP_1_SAFE"] | DEFAULT_TEMP_1_SAFE;
        config.TEMP_2_SAFE = doc["TEMP_2_SAFE"] | DEFAULT_TEMP_2_SAFE;
        config.DUAL_BOILER = doc["DUAL_BOILER"] | DEFAULT_DUAL_BOILER;
        config.TARGET_TEMP_B2 = doc["TARGET_TEMP_B2"] | DEFAULT_TARGET_TEMP_B2;
        config.PID_CONTROL_B2 = doc["PID_CONTROL_B2"] | DEFAULT_PID_CONTROL_B2;
        config.RES_SW = doc["RES_SW"] | DEFAULT_RES_SW;
        config.RES_SW_NC = doc["RES_SW_NC"] | DEFAULT_RES_SW_NC;
        config.WTR_RES_PROBE = doc["WTR_RES_PROBE"] | DEFAULT_WTR_RES_PROBE;
        config.US_LVL_DETECT = doc["US_LVL_DETECT"] | DEFAULT_US_LVL_DETECT;
        config.B2_REFILL_TRIG = doc["B2_REFILL_TRIG"] | DEFAULT_B2_REFILL_TRIG;
        config.B1_WTR_PROBE = doc["B1_WTR_PROBE"] | DEFAULT_B1_WTR_PROBE;
        config.B2_WTR_PROBE = doc["B2_WTR_PROBE"] | DEFAULT_B2_WTR_PROBE;
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

        StaticJsonDocument<1024> doc;
        // Set each setting to the document
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
        doc["SERIAL_PRINTING"] = config.SERIAL_PRINTING;
        doc["ULTRASONIC_SENSOR"] = config.ULTRASONIC_SENSOR;
        doc["TEMP_SENSOR_1"] = config.TEMP_SENSOR_1;
        doc["TEMP_SENSOR_2"] = config.TEMP_SENSOR_2;
        doc["TRANSDUCER_2"] = config.TRANSDUCER_2;
        doc["TRANSDUCER_2_KPA_SAFE"] = config.TRANSDUCER_2_KPA_SAFE;
        doc["TEMP_1_SAFE"] = config.TEMP_1_SAFE;
        doc["TEMP_2_SAFE"] = config.TEMP_2_SAFE;
        doc["DUAL_BOILER"] = config.DUAL_BOILER;
        doc["TARGET_TEMP_B2"] = config.TARGET_TEMP_B2;
        doc["PID_CONTROL_B2"] = config.PID_CONTROL_B2;
        doc["RES_SW"] = config.RES_SW;
        doc["RES_SW_NC"] = config.RES_SW_NC;
        doc["WTR_RES_PROBE"] = config.WTR_RES_PROBE;
        doc["US_LVL_DETECT"] = config.US_LVL_DETECT;
        doc["B2_REFILL_TRIG"] = config.B2_REFILL_TRIG;
        doc["B1_WTR_PROBE"] = config.B1_WTR_PROBE;
        doc["B2_WTR_PROBE"] = config.B2_WTR_PROBE;
        
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


void formatIfMountFailed() {
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

// Serial input confirmation of new settings applied
double parseDouble(const String& input, const String& command) {
    String valueStr = input.substring(command.length());
    double value = valueStr.toDouble();
    if (value > 0) {
        Serial.print("Updating ");
        Serial.print(command);
        Serial.print("to ");
        Serial.println(value);
    } else {
        Serial.println("Invalid value");
    }
    return value;
}

ConfigurationManager configManager; // Define configManager globally

void printFileContents() {
    fs::File file = LittleFS.open(CONFIG_FILE_PATH, FILE_READ);
    if (!file) {
        Serial.println("Failed to open file for reading.");
        return;
    }

    Serial.println("File contents:");
    while (file.available()) {
        char ch = file.read(); // Read the next character
        Serial.write(ch); // Print the character
        if (ch == ',') {
            Serial.println(); // After a comma, move to the next line
        }
    }
    file.close();
}

void sendSettings() {
    uart_write_bytes(UART_NUM_1, "SETTINGS_START\n", strlen("SETTINGS_START\n"));

    fs::File file = LittleFS.open(CONFIG_FILE_PATH, FILE_READ);
    if (!file) {
        uart_write_bytes(UART_NUM_1, "Failed to open settings file.\n", strlen("Failed to open settings file.\n"));
    } else {
        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, file);
        if (!error) {
            for (JsonPair kv : doc.as<JsonObject>()) {
                String line = String(kv.key().c_str()) + ": " + kv.value().as<String>() + "\n";
                uart_write_bytes(UART_NUM_1, line.c_str(), line.length());
            }
        }
        file.close();
    }
    
    uart_write_bytes(UART_NUM_1, "SETTINGS_END\n", strlen("SETTINGS_END\n"));
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

bool HANDSHAKE = false;   // Flag to pause setup until the Slave esp32 has received "HEARTBEAT" message

// ========== SEMAPHORES ========== 
SemaphoreHandle_t xMutexPressure; // Mutex for kPa pressure value
SemaphoreHandle_t xMutexTemprature1; // Mutex for TEMP_C_1 veriable
SemaphoreHandle_t xMutexTemprature2; // Mutex for TEMP_C_2 veriable
SemaphoreHandle_t xMutexCurrentError; // Mutex for checking the current error state
SemaphoreHandle_t xMutexWaterResPercentage; // Mutex for ultrasonic water percentage value


// Declaration of task handles
TaskHandle_t UartHandle = NULL;
TaskHandle_t SerialInputHandle = NULL;
TaskHandle_t BTwoPressureHandle = NULL;
TaskHandle_t UltraSensorHandle = NULL;
TaskHandle_t SerialOutputHandle = NULL;
TaskHandle_t TempSensorHandle = NULL;
TaskHandle_t ResWtrCheckHandle = NULL;
TaskHandle_t BoilerWtrTaskHandle = NULL;
TaskHandle_t SafeModeHandle = NULL;
TaskHandle_t PWMB2ElementTaskHandle = NULL;
TaskHandle_t PIDB2ElementTaskHandle = NULL;

// Stop tasks safely to update/change settings
void STOP_TASKS() {
    if (SerialInputHandle != NULL) {
        vTaskDelete(SerialInputHandle);
    }
    if (BTwoPressureHandle != NULL) {
        vTaskDelete(BTwoPressureHandle);
    }
    if (UltraSensorHandle != NULL) {
        vTaskDelete(UltraSensorHandle);
    }
    if (SerialOutputHandle != NULL) {
        vTaskDelete(SerialOutputHandle);
    }
    if (TempSensorHandle != NULL) {
        vTaskDelete(TempSensorHandle);
    }
    if (ResWtrCheckHandle != NULL) {
        vTaskDelete(ResWtrCheckHandle);
    }
    if (BoilerWtrTaskHandle != NULL) {
        vTaskDelete(BoilerWtrTaskHandle);
    }
    if (SafeModeHandle != NULL) {
        vTaskDelete(SafeModeHandle);
    }
    if (PWMB2ElementTaskHandle != NULL) {
        vTaskDelete(PWMB2ElementTaskHandle);
    }
    if (PIDB2ElementTaskHandle != NULL) {
        vTaskDelete(PIDB2ElementTaskHandle);
    }
}

// ========== TEMPERATURE SENSOR ==========
double TEMP_C_1; // Global variable for the temp 1
double TEMP_C_2; // Global variable for the temp 2

#define CS_PIN_1  21    // CS pin for temp sensor 1
#define CS_PIN_2  14    // CS pin for temp sensor 2
// MOSI, MISO, and SCK pins are shared
#define MOSI_PIN 23   
#define MISO_PIN 19   
#define SCK_PIN  18   

#define RREF      390
#define RNOMINAL  100.0

// Initialize two sensor instances
Adafruit_MAX31865 maxSensor1 = Adafruit_MAX31865(CS_PIN_1, MISO_PIN, MOSI_PIN, SCK_PIN);
Adafruit_MAX31865 maxSensor2 = Adafruit_MAX31865(CS_PIN_2, MISO_PIN, MOSI_PIN, SCK_PIN);


// ========== ULTRASONIC SENSOR ==========
long ULTRA_WATER_PERCENTAGE = 100; // Global variable for water percentage
#define ULTRA_TRIGGER_PIN 25   // Ultrasonic sensor trigger pin (yellow)
#define ULTRA_ECHO_PIN 26      // Ultrasonic sensor echo pin (white)
#define MEASUREMENTS_COUNT 5  // Number of measurements for averaging

volatile long pulseDuration = 0;
volatile bool pulseComplete = false;

// Circular buffer for averaging
static long measurements[MEASUREMENTS_COUNT] = {0}; 
static int US_INDEX = 0; 

// Interrupt service routine for echo pin
void IRAM_ATTR echo_isr() {
    static unsigned long startTime = 0;
    if (digitalRead(ULTRA_ECHO_PIN) == HIGH) {
        startTime = micros();
    } else {
        pulseDuration = micros() - startTime;
        pulseComplete = true;
    }
}

// =============== PID ELEMENT CONTROL ===============
AutoPIDRelay* myAutoPIDRelay = nullptr;  // Pointer for AutoPIDRelay
double pulseWidth = 1000;
bool relayState = false;
double* E2_TARGET;
double* E2_INPUT;
double PRESSURE_KPA_2; // Global variable for calculated pressure


// ========== BOILER 1 WATER LEVEL PROBE ==========
#define BOILER_1_PROBE_PIN T9 // (GPIO32) Boiler 1 water level probe pin
//volatile bool BOILER_1_REFILL = false; // Global flag for boiler refill
volatile int SP_BOILER_1_PROBE = 0; // Serial printing variable for the boiler probe
volatile int BOILER_1_PROBE_TOUCH_VALUE = 0; // Global veriable to hold the value of the probe

// ========== BOILER 2 WATER LEVEL PROBE ==========
#define BOILER_2_PROBE_PIN T8 // (GPIO33) Boiler 2 water level probe pin
volatile bool BOILER_2_REFILL = false; // Global flag for boiler refill
volatile int SP_BOILER_2_PROBE = 0; // Serial printing variable for the boiler probe
volatile int BOILER_2_PROBE_TOUCH_VALUE = 0; // Global veriable to hold the value of the probe

#define BUFFER_SIZE 5
// Circular buffer and index for Boiler 1
int boiler1Buffer[BUFFER_SIZE] = {0};
int boiler1Index = 0;

// Circular buffer and index for Boiler 2
int boiler2Buffer[BUFFER_SIZE] = {0};
int boiler2Index = 0;

// Function to calculate the running average
int calculateRunningAverage(int *buffer) {
    int sum = 0;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        sum += buffer[i];
    }
    return sum / BUFFER_SIZE;
}

// ========== SAFE-MODE ==========
enum ErrorCode {
    NO_ERROR,
    PRESSURE_SENSOR_OUT_OF_RANGE_B2,
    TEMP_SENSOR_1_SM,
    TEMP_SENSOR_2_SM,
    // Commnication
    LOST_UART_CONNECTION,
};
ErrorCode currentError = NO_ERROR;

volatile bool ENTER_SAFE_MODE = false; // Global flag for error state


// ========== PRESSURE TRANSDUCER 2 ==========
const int PRESSURE_SENSOR_PIN = 35;     // Pressure transducer sensor pin
volatile bool CALIBRATING_B2 = false; // Flag to indicate if Boiler 2 is being calibrated
#define VOLTAGE_DIVIDER_RATIO 1.51 // Voltage devider ratio used to lower the transducer signal

// Buffer to hold the last 5 voltage readings
float voltageReadings[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
int readingIndex = 0;




// =============== PWM ELEMENT CONTROL ===============
double ELEMENT;
#define ELEMENT_2 27 // Boiler heating element pin


// =============== TASKS ===============

// RESERVOIR WATER LEVEL TASK
extern void RES_WTR_TASK(void *pvParameters);

// ULTRASONIC SENSOR TASK
extern void ULTRASONIC_SENSOR_TASK(void *pvParameters);

// SAFE-MODE TASK
extern void SAFE_MODE_TASK(void *pvParameters);

// BOILER WATER LEVEL CHECK TASK
extern void BOILER_WTR_TASK(void *pvParameters);

// TEMP SENSOR TASK
extern void READ_TEMP_TASK(void *pvParameters);

// READING PRESSURE TASK
extern void READ_PRESSURE_TASK(void *pvParameters);

// UART TASK
extern void UART_TASK(void *pvParameters);

// SERIAL INPUT TASK
extern void SERIAL_INPUT_TASK(void *pvParameters);

// BOILER 2 PID/PWM ELEMENT CONTROL
extern void PWM_B2_ELEMENT_TASK(void *pvParameters);
extern void PID_B2_ELEMENT_TASK(void *pvParameters);

void setup() {
    Serial.begin(115200);
    delay(1000); // Wait for serial monitor to open
    formatIfMountFailed();
    delay(500); // Wait for format
    configManager = ConfigurationManager(); // Ensures LittleFS is checked and formatted if necessary

    // UART
    INITIALIZE_UART();

    // START UART TASK
    xTaskCreatePinnedToCore(
        UART_TASK,        // Task function
        "UART Task",      // Name of the task (for debugging)
        4096,             // Stack size in words
        NULL,             // Task input parameter
        5,                // Priority of the task
        &UartHandle,      // Task handle
        0                 // Core where the task should run
    );

    // WAIT FOR THE MASTER ESP32'S HANDSHAKE
    while (!HANDSHAKE) {
        delay(100); // Wait for 100 milliseconds before checking the flag again
    }

    // CONFIG PRESSURE OR TEMP TARGET FOR PID/PWM ELEMENT CONTROL
    if (configManager.config.TARGET_TEMP_B2) {
        E2_TARGET = &configManager.config.B2_TEMP;
        E2_INPUT = &TEMP_C_2;
    } else {
        E2_TARGET = &configManager.config.B2_KPA;
        E2_INPUT = &PRESSURE_KPA_2;
    }

    // Dynamically create the AutoPIDRelay instance
    myAutoPIDRelay = new AutoPIDRelay(E2_INPUT, E2_TARGET, &relayState, pulseWidth, configManager.config.B2_KP, configManager.config.B2_KI, configManager.config.B2_KD);

    pinMode(ELEMENT_2, OUTPUT);
    digitalWrite(ELEMENT_2, HIGH);

    // Ultrasonic sensor
    if (configManager.config.ULTRASONIC_SENSOR) {
        pinMode(ULTRA_TRIGGER_PIN, OUTPUT);
        pinMode(ULTRA_ECHO_PIN, INPUT);
        // Attach interrupt to the echo pin
        attachInterrupt(digitalPinToInterrupt(ULTRA_ECHO_PIN), echo_isr, CHANGE);
    }

    // MAX31865 temp sensors
    if (configManager.config.TEMP_SENSOR_1) {
        if (!maxSensor1.begin(MAX31865_3WIRE) || !maxSensor2.begin(MAX31865_3WIRE)) {
            Serial.println("Failed to initialize one or more temperature sensors.");
            while (1);
        }
        maxSensor1.setWires(MAX31865_3WIRE);
        maxSensor1.enableBias(true);
        maxSensor1.autoConvert(true);
    }

    if (configManager.config.TEMP_SENSOR_2) {
        maxSensor2.setWires(MAX31865_3WIRE);
        maxSensor2.enableBias(true);
        maxSensor2.autoConvert(true);
    }

    if (configManager.config.RES_SW) {
        pinMode(RESERVOIR_PIN, INPUT_PULLUP);
    }



    // CREATE MUTEX
    xMutexTemprature1  = xSemaphoreCreateMutex(); // TEMP_C_1
    xMutexTemprature2  = xSemaphoreCreateMutex(); // TEMP_C_2
    xMutexCurrentError = xSemaphoreCreateMutex(); // Safe-mode current error state
    xMutexPressure = xSemaphoreCreateMutex(); // Pressure reading
    xMutexWaterResPercentage = xSemaphoreCreateMutex(); // Ultrasonic sensor water percentage



    // CREATE TASKS
    // Run on core 0
    xTaskCreatePinnedToCore(
        SERIAL_INPUT_TASK,      // Task function
        "Serial Input Task",    // Name of the task (for debugging)
        4096,                   // Stack size in words
        NULL,                   // Task input parameter
        1,                      // Priority of the task
        &SerialInputHandle,     // Task handle
        0                       // Core where the task should run
    ); 

    // Run on core 1
    if (configManager.config.TRANSDUCER_2) {
        xTaskCreatePinnedToCore(
            READ_PRESSURE_TASK,      // Task function
            "Read Pressure",         // Name of the task (for debugging)
            2048,                    // Stack size in words
            NULL,                    // Task input parameter
            1,                       // Priority of the task
            &BTwoPressureHandle,     // Task handle
            1                        // Core where the task should run
        );
    }

    if (configManager.config.ULTRASONIC_SENSOR) {
        xTaskCreatePinnedToCore(
            ULTRASONIC_SENSOR_TASK,     // Task function
            "Ultrasonic Sensor",        // Name of the task (for debugging)
            2048,                       // Stack size in words
            NULL,                       // Task input parameter
            1,                          // Priority of the task
            &UltraSensorHandle,         // Task handle
            1                           // Core where the task should run
        );    
    }


    if (configManager.config.DUAL_BOILER) {
        if (configManager.config.PID_CONTROL_B2) {
            xTaskCreatePinnedToCore(
                PID_B2_ELEMENT_TASK,        // Task function
                "PID B2 element task",      // Name of the task (for debugging)
                2048,                       // Stack size in words
                NULL,                       // Task input parameter
                1,                          // Priority of the task
                &PIDB2ElementTaskHandle,    // Task handle
                1                           // Core where the task should run
            );  
        } else {
            xTaskCreatePinnedToCore(
                PWM_B2_ELEMENT_TASK,      // Task function
                "PWM B2 element task",    // Name of the task (for debugging)
                2048,                     // Stack size in words
                NULL,                     // Task input parameter
                1,                        // Priority of the task
                &PWMB2ElementTaskHandle,  // Task handle
                1                         // Core where the task should run
            );  
        }
    }

    if (configManager.config.TEMP_SENSOR_1) {
        xTaskCreatePinnedToCore(
            READ_TEMP_TASK,       // Task function
            "Read Temperature",   // Name of the task (for debugging)
            2048,                 // Stack size in words
            NULL,                 // Task input parameter
            1,                    // Priority of the task
            &TempSensorHandle,    // Task handle
            1                     // Core where the task should run
        ); 
    }

    xTaskCreatePinnedToCore(
        RES_WTR_TASK,             // Task function
        "Res water level task",   // Name of the task (for debugging)
        2048,                     // Stack size in words
        NULL,                     // Task input parameter
        1,                        // Priority of the task
        &ResWtrCheckHandle,       // Task handle
        1                         // Core where the task should run
    );

    xTaskCreatePinnedToCore(
        BOILER_WTR_TASK,            // Task function
        "Boiler water level task",  // Name of the task (for debugging)
        2048,                       // Stack size in words
        NULL,                       // Task input parameter
        1,                          // Priority of the task
        &BoilerWtrTaskHandle,       // Task handle
        1                           // Core where the task should run
    );

    xTaskCreatePinnedToCore(
        SAFE_MODE_TASK,               // Task function
        "Check and Safe Mode task",   // Name of the task (for debugging)
        4096,                         // Stack size in words
        NULL,                         // Task input parameter
        5,                            // Priority of the task
        &SafeModeHandle,              // Task handle
        1                             // Core where the task should run
    );

}


void loop() {

}