/**
 * Simple DS18B20 Temperature Sensor Driver for Raspberry Pi Pico
 * Written in pure C for maximum compatibility
 * 
 * Copyright (c) 2024 Peter Westlund
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DS18B20_H
#define DS18B20_H

#include <stdint.h>
#include <stdbool.h>

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
 * Initialize DS18B20 sensor on specified GPIO pin
 * 
 * @param gpio_pin GPIO pin number for 1-Wire bus
 * @return DS18B20_OK if successful, error code otherwise
 */
ds18b20_result_t ds18b20_init(unsigned int gpio_pin);

/**
 * Read temperature from DS18B20 sensor
 * 
 * @param temperature_c Pointer to store temperature in Celsius
 * @return DS18B20_OK if successful, error code otherwise
 */
ds18b20_result_t ds18b20_read_temperature(float *temperature_c);

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
const char* ds18b20_error_string(ds18b20_result_t result);

#ifdef __cplusplus
}
#endif

#endif // DS18B20_H
