/**
 * Simple DS18B20 Temperature Sensor Driver for Raspberry Pi Pico
 * Written in pure C for maximum compatibility and timing precision
 * 
 * Copyright (c) 2024 Peter Westlund
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ds18b20.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"
#include "pico/time.h"

static unsigned int ds18b20_gpio = 2;

// DS18B20 Commands
#define DS18B20_SKIP_ROM        0xCC
#define DS18B20_CONVERT_T       0x44
#define DS18B20_READ_SCRATCHPAD 0xBE

// Timing constants (microseconds) - conservative values for reliability
#define DS18B20_RESET_PULSE     500   // Reset pulse duration
#define DS18B20_PRESENCE_WAIT   70    // Wait for presence pulse
#define DS18B20_PRESENCE_READ   240   // Presence detection window
#define DS18B20_WRITE_1_LOW     6     // Write 1: low time
#define DS18B20_WRITE_1_HIGH    64    // Write 1: high time  
#define DS18B20_WRITE_0_LOW     60    // Write 0: low time
#define DS18B20_WRITE_0_HIGH    10    // Write 0: high time
#define DS18B20_READ_LOW        6     // Read: initial low time
#define DS18B20_READ_SAMPLE     9     // Read: sample timing
#define DS18B20_READ_HIGH       55    // Read: remaining high time

/**
 * Initialize DS18B20 sensor
 */
ds18b20_result_t ds18b20_init(unsigned int gpio_pin) {
    ds18b20_gpio = gpio_pin;
    
    // Initialize GPIO with pull-up
    gpio_init(ds18b20_gpio);
    gpio_set_dir(ds18b20_gpio, GPIO_IN);
    gpio_pull_up(ds18b20_gpio);
    
    // Small delay for GPIO to settle
    sleep_ms(10);
    
    // Test if sensor responds
    uint32_t ints = save_and_disable_interrupts();
    
    // Reset pulse
    gpio_set_dir(ds18b20_gpio, GPIO_OUT);
    gpio_put(ds18b20_gpio, 0);
    busy_wait_us(DS18B20_RESET_PULSE);
    
    // Release and wait for presence
    gpio_set_dir(ds18b20_gpio, GPIO_IN);
    busy_wait_us(DS18B20_PRESENCE_WAIT);
    
    // Check for presence pulse
    bool presence = !gpio_get(ds18b20_gpio);
    
    // Wait for presence to end
    busy_wait_us(DS18B20_PRESENCE_READ);
    
    restore_interrupts(ints);
    
    return presence ? DS18B20_OK : DS18B20_ERROR_NO_DEVICE;
}

/**
 * Send reset pulse and check for presence
 */
static bool ds18b20_reset(void) {
    uint32_t ints = save_and_disable_interrupts();
    
    // Reset pulse
    gpio_set_dir(ds18b20_gpio, GPIO_OUT);
    gpio_put(ds18b20_gpio, 0);
    busy_wait_us(DS18B20_RESET_PULSE);
    
    // Release and wait for presence
    gpio_set_dir(ds18b20_gpio, GPIO_IN);
    busy_wait_us(DS18B20_PRESENCE_WAIT);
    
    // Check for presence pulse
    bool presence = !gpio_get(ds18b20_gpio);
    
    // Wait for presence to end
    busy_wait_us(DS18B20_PRESENCE_READ);
    
    restore_interrupts(ints);
    
    return presence;
}

/**
 * Write a single bit
 */
static void ds18b20_write_bit(bool bit) {
    uint32_t ints = save_and_disable_interrupts();
    
    if (bit) {
        // Write 1: short low pulse
        gpio_set_dir(ds18b20_gpio, GPIO_OUT);
        gpio_put(ds18b20_gpio, 0);
        busy_wait_us(DS18B20_WRITE_1_LOW);
        gpio_set_dir(ds18b20_gpio, GPIO_IN);
        busy_wait_us(DS18B20_WRITE_1_HIGH);
    } else {
        // Write 0: long low pulse
        gpio_set_dir(ds18b20_gpio, GPIO_OUT);
        gpio_put(ds18b20_gpio, 0);
        busy_wait_us(DS18B20_WRITE_0_LOW);
        gpio_set_dir(ds18b20_gpio, GPIO_IN);
        busy_wait_us(DS18B20_WRITE_0_HIGH);
    }
    
    restore_interrupts(ints);
}

/**
 * Read a single bit
 */
static bool ds18b20_read_bit(void) {
    uint32_t ints = save_and_disable_interrupts();
    
    // Start read slot
    gpio_set_dir(ds18b20_gpio, GPIO_OUT);
    gpio_put(ds18b20_gpio, 0);
    busy_wait_us(DS18B20_READ_LOW);
    
    // Release and sample
    gpio_set_dir(ds18b20_gpio, GPIO_IN);
    busy_wait_us(DS18B20_READ_SAMPLE);
    bool bit = gpio_get(ds18b20_gpio);
    
    // Complete read slot
    busy_wait_us(DS18B20_READ_HIGH);
    
    restore_interrupts(ints);
    
    return bit;
}

/**
 * Write a byte
 */
static void ds18b20_write_byte(uint8_t byte) {
    for (int i = 0; i < 8; i++) {
        ds18b20_write_bit(byte & (1 << i));
    }
}

/**
 * Read a byte
 */
static uint8_t ds18b20_read_byte(void) {
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++) {
        if (ds18b20_read_bit()) {
            byte |= (1 << i);
        }
    }
    return byte;
}

/**
 * Read temperature from DS18B20
 */
ds18b20_result_t ds18b20_read_temperature(float *temperature_c) {
    if (!temperature_c) {
        return DS18B20_ERROR_TIMEOUT;
    }
    
    // Initialize to safe value
    *temperature_c = 0.0f;
    
    // Check if sensor is present
    if (!ds18b20_reset()) {
        return DS18B20_ERROR_NO_DEVICE;
    }
    
    // Start temperature conversion
    ds18b20_write_byte(DS18B20_SKIP_ROM);
    ds18b20_write_byte(DS18B20_CONVERT_T);
    
    // Wait for conversion (750ms for 12-bit resolution)
    sleep_ms(750);
    
    // Read scratchpad
    if (!ds18b20_reset()) {
        return DS18B20_ERROR_NO_DEVICE;
    }
    
    ds18b20_write_byte(DS18B20_SKIP_ROM);
    ds18b20_write_byte(DS18B20_READ_SCRATCHPAD);
    
    // Read temperature bytes (we only need first 2 bytes)
    uint8_t temp_lsb = ds18b20_read_byte();
    uint8_t temp_msb = ds18b20_read_byte();
    
    // Skip remaining bytes for simplicity
    for (int i = 2; i < 9; i++) {
        ds18b20_read_byte();
    }
    
    // Combine temperature bytes
    int16_t temp_raw = (temp_msb << 8) | temp_lsb;
    
    // Check for valid reading
    if (temp_raw == 0x0550 || temp_raw == 0xFFFF || temp_raw == 0x0000) {
        return DS18B20_ERROR_CRC;
    }
    
    // Convert to Celsius (12-bit resolution: LSB = 0.0625°C)
    *temperature_c = temp_raw / 16.0f;
    
    // Sanity check temperature range
    if (*temperature_c < -55.0f || *temperature_c > 125.0f) {
        return DS18B20_ERROR_CRC;
    }
    
    return DS18B20_OK;
}

/**
 * Convert Celsius to Fahrenheit
 */
float ds18b20_celsius_to_fahrenheit(float celsius) {
    return celsius * 9.0f / 5.0f + 32.0f;
}

/**
 * Get error string for result code
 */
const char* ds18b20_error_string(ds18b20_result_t result) {
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
