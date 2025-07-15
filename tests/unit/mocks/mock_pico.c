#include "mock_pico.h"
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

// Mock time tracking
static uint64_t mock_time_us = 0;
static bool mock_time_auto_increment = true;

void mock_pico_reset(void) {
    mock_time_us = 0;
    mock_time_auto_increment = true;
    printf("Mock Pico SDK reset\n");
}

// Mock time functions
uint64_t time_us_64(void) {
    if (mock_time_auto_increment) {
        mock_time_us += 1000; // Auto-increment by 1ms
    }
    return mock_time_us;
}

uint32_t time_us_32(void) {
    return (uint32_t) time_us_64();
}

absolute_time_t get_absolute_time(void) {
    absolute_time_t t;
    t._private_us_since_boot = time_us_64();
    return t;
}

int64_t absolute_time_diff_us(absolute_time_t from, absolute_time_t to) {
    return to._private_us_since_boot - from._private_us_since_boot;
}

// Mock delay functions
void sleep_ms(uint32_t ms) {
    printf("Mock: sleep_ms(%u)\n", ms);
    if (mock_time_auto_increment) {
        mock_time_us += ms * 1000;
    }
    // For testing, we don't actually sleep
    // usleep(ms * 1000); // Uncomment if you want real delays in tests
}

void sleep_us(uint64_t us) {
    printf("Mock: sleep_us(%llu)\n", (unsigned long long) us);
    if (mock_time_auto_increment) {
        mock_time_us += us;
    }
    // usleep(us); // Uncomment if you want real delays in tests
}

void busy_wait_us(uint64_t us) {
    printf("Mock: busy_wait_us(%llu)\n", (unsigned long long) us);
    if (mock_time_auto_increment) {
        mock_time_us += us;
    }
}

// Mock stdio functions
void stdio_init_all(void) {
    printf("Mock: stdio_init_all()\n");
}

// Mock control functions
void mock_set_time_us(uint64_t us) {
    mock_time_us = us;
}

uint64_t mock_get_time_us(void) {
    return mock_time_us;
}

void mock_set_auto_increment_time(bool enable) {
    mock_time_auto_increment = enable;
}
