#ifndef DS18B20_UTILS_H
#define DS18B20_UTILS_H

/**
 * Testable utility functions from DS18B20 driver
 * These functions don't depend on hardware and can be tested on Linux
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * DS18B20 result codes
 */
typedef enum {
    DS18B20_OK = 0,
    DS18B20_ERROR_NO_DEVICE = -1,
    DS18B20_ERROR_CRC = -2,
    DS18B20_ERROR_TIMEOUT = -3
} ds18b20_result_t;

/**
 * Convert Celsius to Fahrenheit
 *
 * @param celsius Temperature in Celsius
 * @return Temperature in Fahrenheit
 */
float ds18b20_celsius_to_fahrenheit(float celsius);

/**
 * Get human-readable error string
 *
 * @param result Result code
 * @return Error description string
 */
const char *ds18b20_error_string(ds18b20_result_t result);

#ifdef __cplusplus
}
#endif

#endif // DS18B20_UTILS_H
