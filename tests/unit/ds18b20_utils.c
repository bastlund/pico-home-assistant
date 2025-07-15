#include "ds18b20_utils.h"

/**
 * Convert Celsius to Fahrenheit
 */
float ds18b20_celsius_to_fahrenheit(float celsius) {
    return celsius * 9.0f / 5.0f + 32.0f;
}

/**
 * Get error string for result code
 */
const char *ds18b20_error_string(ds18b20_result_t result) {
    switch (result) {
        case DS18B20_OK:
            return "Success";
        case DS18B20_ERROR_NO_DEVICE:
            return "No device found";
        case DS18B20_ERROR_CRC:
            return "Invalid reading";
        case DS18B20_ERROR_TIMEOUT:
            return "Timeout or invalid parameter";
        default:
            return "Unknown error";
    }
}
