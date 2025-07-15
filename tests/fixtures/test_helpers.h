#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

/**
 * Test helpers and utilities for pico-home-assistant tests
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize test helpers
 */
void test_helpers_init(void);

/**
 * Clean up test helpers
 */
void test_helpers_cleanup(void);

/**
 * Assert that a float value is within a specified tolerance of expected value
 * @param actual The actual value
 * @param expected The expected value
 * @param tolerance The acceptable tolerance
 */
void assert_float_within_range(float actual, float expected, float tolerance);

/**
 * Assert that a temperature reading is within valid DS18B20 range
 * @param temperature Temperature value to validate
 */
void assert_temperature_valid(float temperature);

/**
 * Print a test banner for better output formatting
 * @param test_name Name of the test being run
 */
void print_test_banner(const char *test_name);

#ifdef __cplusplus
}
#endif

#endif // TEST_HELPERS_H
