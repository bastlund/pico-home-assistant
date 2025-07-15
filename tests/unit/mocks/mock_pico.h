#ifndef MOCK_PICO_H
#define MOCK_PICO_H

/**
 * Mock implementation of Pico SDK core functions for testing on Linux
 */

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Types from Pico SDK
typedef struct {
    uint64_t _private_us_since_boot;
} absolute_time_t;

// Mock control functions
void mock_pico_reset(void);
void mock_set_time_us(uint64_t us);
uint64_t mock_get_time_us(void);
void mock_set_auto_increment_time(bool enable);

// Mock implementations of Pico SDK functions
uint64_t time_us_64(void);
uint32_t time_us_32(void);
absolute_time_t get_absolute_time(void);
int64_t absolute_time_diff_us(absolute_time_t from, absolute_time_t to);

void sleep_ms(uint32_t ms);
void sleep_us(uint64_t us);
void busy_wait_us(uint64_t us);

void stdio_init_all(void);

#ifdef __cplusplus
}
#endif

#endif // MOCK_PICO_H
