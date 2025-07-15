#include "test_helpers.h"
#include "unity.h"
#include <stdio.h>
#include <string.h>

void test_helpers_init(void) {
    // Initialize any global test state here
    printf("Test helpers initialized\n");
}

void test_helpers_cleanup(void) {
    // Clean up any global test state here
    printf("Test helpers cleaned up\n");
}

void assert_float_within_range(float actual, float expected, float tolerance) {
    float diff = actual - expected;
    if (diff < 0)
        diff = -diff; // abs() for floats

    if (diff > tolerance) {
        char message[256];
        snprintf(message, sizeof(message), "Expected %.3f, but was %.3f (tolerance: %.3f)",
                 expected, actual, tolerance);
        TEST_FAIL_MESSAGE(message);
    }
}

void assert_temperature_valid(float temperature) {
    // DS18B20 valid range is -55°C to +125°C
    if (temperature < -55.0f || temperature > 125.0f) {
        char message[128];
        snprintf(message, sizeof(message),
                 "Temperature %.2f°C is outside valid DS18B20 range (-55°C to +125°C)",
                 temperature);
        TEST_FAIL_MESSAGE(message);
    }
}

void print_test_banner(const char *test_name) {
    printf("\n==================================================\n");
    printf("Testing: %s\n", test_name);
    printf("==================================================\n");
}
