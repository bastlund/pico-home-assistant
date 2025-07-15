#include "unity.h"
#include "test_helpers.h"
#include "mock_pico.h"

// Test utility functions and general helpers
#include <string.h>
#include <stdio.h>

void setUp(void) {
    mock_pico_reset();
}

void tearDown(void) {
    // Clean up after each test
}

/**
 * Test string utilities (if we have any)
 */
void test_string_utilities(void) {
    print_test_banner("String Utilities");

    // Test basic string operations
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "Temperature: %.2f°C", 25.75f);

    TEST_ASSERT_NOT_NULL(strstr(buffer, "25.75"));
    TEST_ASSERT_NOT_NULL(strstr(buffer, "Temperature"));

    printf("String test: %s\n", buffer);
}

/**
 * Test mock time functions
 */
void test_mock_time_functions(void) {
    print_test_banner("Mock Time Functions");

    // Test time progression
    uint64_t start_time = mock_get_time_us();
    sleep_ms(100);
    uint64_t end_time = mock_get_time_us();

    // Should have advanced by at least 100ms (100,000 us)
    TEST_ASSERT_GREATER_OR_EQUAL(100000, end_time - start_time);

    // Test absolute time
    absolute_time_t t1 = get_absolute_time();
    sleep_ms(50);
    absolute_time_t t2 = get_absolute_time();

    int64_t diff = absolute_time_diff_us(t1, t2);
    TEST_ASSERT_GREATER_OR_EQUAL(50000, diff);

    printf("Time diff: %lld us\n", (long long) diff);
}

/**
 * Test version string formatting (example utility test)
 */
void test_version_formatting(void) {
    print_test_banner("Version Formatting");

    // Simulate version formatting
    char version_str[32];
    int major = 0, minor = 1, patch = 3;

    snprintf(version_str, sizeof(version_str), "%d.%d.%d", major, minor, patch);

    TEST_ASSERT_EQUAL_STRING("0.1.3", version_str);

    printf("Version string: %s\n", version_str);
}

/**
 * Test temperature conversion edge cases
 */
void test_temperature_range_validation(void) {
    print_test_banner("Temperature Range Validation");

    // Test various temperature ranges
    struct {
        float temp;
        bool should_be_valid;
    } test_cases[] = {
        {-55.0f, true},  // DS18B20 minimum
        {-56.0f, false}, // Below DS18B20 minimum
        {125.0f, true},  // DS18B20 maximum
        {126.0f, false}, // Above DS18B20 maximum
        {0.0f, true},    // Freezing point
        {100.0f, true},  // Boiling point
        {25.0f, true},   // Room temperature
    };

    int num_tests = sizeof(test_cases) / sizeof(test_cases[0]);
    int valid_count = 0;

    for (int i = 0; i < num_tests; i++) {
        bool is_valid = (test_cases[i].temp >= -55.0f && test_cases[i].temp <= 125.0f);

        TEST_ASSERT_EQUAL(test_cases[i].should_be_valid, is_valid);

        if (is_valid) {
            valid_count++;
        }

        printf("Temperature %.1f°C: %s\n", test_cases[i].temp, is_valid ? "VALID" : "INVALID");
    }

    printf("Valid temperatures: %d/%d\n", valid_count, num_tests);
}

/**
 * Test helper function reliability
 */
void test_helper_functions(void) {
    print_test_banner("Helper Function Reliability");

    // Test that our test helpers work correctly
    assert_float_within_range(1.0f, 1.0f, 0.01f);   // Exact match
    assert_float_within_range(1.001f, 1.0f, 0.01f); // Within tolerance

    // Test temperature validation
    assert_temperature_valid(20.0f);
    assert_temperature_valid(-50.0f);
    assert_temperature_valid(120.0f);

    printf("All helper function tests passed\n");
}

/**
 * Main test runner
 */
int main(void) {
    UNITY_BEGIN();

    test_helpers_init();

    RUN_TEST(test_string_utilities);
    RUN_TEST(test_mock_time_functions);
    RUN_TEST(test_version_formatting);
    RUN_TEST(test_temperature_range_validation);
    RUN_TEST(test_helper_functions);

    test_helpers_cleanup();

    return UNITY_END();
}
