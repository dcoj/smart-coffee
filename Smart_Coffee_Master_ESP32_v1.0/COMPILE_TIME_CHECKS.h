// ================================================================================
// ============= IT IS NOT RECOMMENDED TO CHANGE ANYTHING ON THIS PAGE ============
// ================================================================================


// DUAL BOILER:
#if DEFAULT_DUAL_BOILER == true && (DEFAULT_TRANSDUCER_2 == false && DEFAULT_TARGET_TEMP_B2 == false)
#error "Configuration Error: When DEFAULT_DUAL_BOILER is enabled, enable at least one of the following: DEFAULT_TRANSDUCER_2 or DEFAULT_TARGET_TEMP_B2."
#endif

// PRESSURE TRANSDUCER 1
#if (DEFAULT_TARGET_TEMP_B1 == false) && (DEFAULT_TRANSDUCER_1 == false)
#error "Configuration Error: Please enable one of the following options: DEFAULT_TARGET_TEMP_B1 or DEFAULT_TRANSDUCER_1 to proceed"
#endif

#if (DEFAULT_TRANSDUCER_1_KPA_SAFE == true) && (DEFAULT_TRANSDUCER_1 == false)
#error "Configuration Error: DEFAULT_TRANSDUCER_1_KPA_SAFE is enabled while DEFAULT_TRANSDUCER_1 is disabled. Please enable DEFAULT_TRANSDUCER_1 to proceed"
#endif

// PRESSURE TRANSDUCER 2
#if DEFAULT_DUAL_BOILER == true && (DEFAULT_TARGET_TEMP_B2 == false) && (DEFAULT_TRANSDUCER_2 == false)
#error "Configuration Error: Please enable one of the following options: DEFAULT_TARGET_TEMP_B2 or DEFAULT_TRANSDUCER_2 to proceed"
#endif

#if (DEFAULT_TRANSDUCER_2_KPA_SAFE == true) && (DEFAULT_TRANSDUCER_2 == false)
#error "Configuration Error: DEFAULT_TRANSDUCER_2_KPA_SAFE is enabled while DEFAULT_TRANSDUCER_2 is disabled. Please enable DEFAULT_TRANSDUCER_2 to proceed"
#endif

// TEMP SENSOR 1
#if (DEFAULT_TARGET_TEMP_B1 == true) && (DEFAULT_TEMP_SENSOR_1 == false)
#error "Configuration Error: DEFAULT_TARGET_TEMP_B1 is enabled while DEFAULT_TEMP_SENSOR_1 is disabled. Please enable DEFAULT_TEMP_SENSOR_1 to proceed"
#endif
#if (DEFAULT_TEMP_1_SAFE == true) && (DEFAULT_TEMP_SENSOR_1 == false)
#error "Configuration Error: DEFAULT_TEMP_1_SAFE is enabled while DEFAULT_TEMP_SENSOR_1 is disabled. Please enable DEFAULT_TEMP_SENSOR_1 to proceed"
#endif

// TEMP SENSOR 2
#if (DEFAULT_TARGET_TEMP_B2 == true) && (DEFAULT_TEMP_SENSOR_2 == false)
#error "Configuration Error: DEFAULT_TARGET_TEMP_B2 is enabled while DEFAULT_TEMP_SENSOR_2 is disabled. Please enable DEFAULT_TEMP_SENSOR_2 to proceed"
#endif
#if (DEFAULT_TEMP_2_SAFE == true) && (DEFAULT_TEMP_SENSOR_2 == false)
#error "Configuration Error: DEFAULT_TEMP_1_SAFE is enabled while DEFAULT_TEMP_SENSOR_2 is disabled. Please enable DEFAULT_TEMP_SENSOR_2 to proceed"
#endif

// ULTRASONIC SENSOR
#if (DEFAULT_US_LVL_DETECT == true) && (DEFAULT_ULTRASONIC_SENSOR == false)
#error "Configuration Error: DEFAULT_US_LVL_DETECT is enabled while DEFAULT_ULTRASONIC_SENSOR is disabled. Please enable DEFAULT_ULTRASONIC_SENSOR to proceed"
#endif

// FLOW METERS
#if (DEFAULT_FLOW_METER_2 == true) && (DEFAULT_FLOW_METER == false)
#error "Configuration Error: DEFAULT_FLOW_METER_2 is enabled while DEFAULT_FLOW_METER is disabled. Please enable DEFAULT_FLOW_METER to proceed"
#endif
#if (DEFAULT_ESPRESSO_VOLUME == true) && (DEFAULT_FLOW_METER == false)
#error "Configuration Error: DEFAULT_ESPRESSO_VOLUME is enabled while DEFAULT_FLOW_METER is disabled. Please enable DEFAULT_FLOW_METER to proceed"
#endif
#if (DEFAULT_FLOW_METER_2_CAL == true) && (DEFAULT_FLOW_METER_2 == false)
#error "Configuration Error: DEFAULT_FLOW_METER_2_CAL is enabled while DEFAULT_FLOW_METER_2 is disabled. Please enable DEFAULT_FLOW_METER_2 to proceed"
#endif

// LOAD CELL
#if (DEFAULT_ESPRESSO_WEIGHT == true) && (DEFAULT_LOAD_CELL == false)
#error "Configuration Error: DEFAULT_ESPRESSO_WEIGHT is enabled while DEFAULT_LOAD_CELL is disabled. Please enable DEFAULT_LOAD_CELL to proceed"
#endif

// RESERVOIR LEVEL DETECTION
#if ((DEFAULT_US_LVL_DETECT ? 1 : 0) + (DEFAULT_WTR_RES_PROBE ? 1 : 0) + (DEFAULT_RES_SW ? 1 : 0)) > 1
#error "Configuration Error: Only 1 of the following options can be enabled at a time: DEFAULT_US_LVL_DETECT, DEFAULT_WTR_RES_PROBE, or DEFAULT_RES_SW."
#endif

// ESPRESSO SHOT
#if ((DEFAULT_ESPRESSO_VOLUME ? 1 : 0) + (DEFAULT_ESPRESSO_WEIGHT ? 1 : 0) + (DEFAULT_ESPRESSO_TIMER ? 1 : 0)) > 1
#error "Configuration Error: Only one of the following options can be enabled at a time: DEFAULT_ESPRESSO_VOLUME, DEFAULT_ESPRESSO_WEIGHT, or DEFAULT_ESPRESSO_TIMER."
#endif

// DISCLAIMER
#if (I_AGREE_TO_THE_TERMS_IN_THE_DISCLAIMER == false)
#error "You did not agree to the disclaimer. To use the software, you must write true at the end of "#define I_AGREE_TO_THE_TERMS_IN_THE_DISCLAIMER" on the config tab"
#endif
