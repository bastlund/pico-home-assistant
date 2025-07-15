#include "unity.h"
#include "test_helpers.h"
#include "mock_gpio.h"
#include "mock_pico.h"
#include <string.h>

// Include testable DS18B20 utilities
#include "ds18b20_utils.h"

void setUp(void) {
    mock_gpio_reset();
    mock_pico_reset();
}

void tearDown(void) {
    // Clean up after each test
}

/**
 * Test DS18B20 Celsius to Fahrenheit conversion
 */
void test_celsius_to_fahrenheit_conversion(void) {
    print_test_banner("Celsius to Fahrenheit Conversion");

    // Test common temperature conversions
    assert_float_within_range(ds18b20_celsius_to_fahrenheit(0.0f), 32.0f, 0.01f);
    assert_float_within_range(ds18b20_celsius_to_fahrenheit(100.0f), 212.0f, 0.01f);
    assert_float_within_range(ds18b20_celsius_to_fahrenheit(-40.0f), -40.0f, 0.01f);
    assert_float_within_range(ds18b20_celsius_to_fahrenheit(25.0f), 77.0f, 0.01f);
    assert_float_within_range(ds18b20_celsius_to_fahrenheit(-10.0f), 14.0f, 0.01f);
}

/**
 * Test DS18B20 error string function
 */
void test_error_strings(void) {
    print_test_banner("Error String Functions");

    const char *ok_str = ds18b20_error_string(DS18B20_OK);
    const char *no_device_str = ds18b20_error_string(DS18B20_ERROR_NO_DEVICE);
    const char *crc_str = ds18b20_error_string(DS18B20_ERROR_CRC);
    const char *timeout_str = ds18b20_error_string(DS18B20_ERROR_TIMEOUT);

    TEST_ASSERT_NOT_NULL(ok_str);
    TEST_ASSERT_NOT_NULL(no_device_str);
    TEST_ASSERT_NOT_NULL(crc_str);
    TEST_ASSERT_NOT_NULL(timeout_str);

    // Verify the strings are different and meaningful
    TEST_ASSERT_NOT_EQUAL(0, strcmp(ok_str, no_device_str));
    TEST_ASSERT_NOT_EQUAL(0, strcmp(ok_str, crc_str));
    TEST_ASSERT_NOT_EQUAL(0, strcmp(no_device_str, timeout_str));

    printf("OK: %s\n", ok_str);
    printf("No Device: %s\n", no_device_str);
    printf("CRC Error: %s\n", crc_str);
    printf("Timeout: %s\n", timeout_str);
}

/**
 * Test temperature validation helper
 */
void test_temperature_validation(void) {
    print_test_banner("Temperature Validation");

    // Valid temperatures should pass
    assert_temperature_valid(25.0f);
    assert_temperature_valid(-10.0f);
    assert_temperature_valid(85.0f);
    assert_temperature_valid(-55.0f); // DS18B20 minimum
    assert_temperature_valid(125.0f); // DS18B20 maximum

    printf("All valid temperature tests passed\n");
}

/**
 * Test temperature edge cases
 */
void test_temperature_edge_cases(void) {
    print_test_banner("Temperature Edge Cases");

    // Test conversion with extreme values
    float very_hot = ds18b20_celsius_to_fahrenheit(125.0f);  // DS18B20 max
    float very_cold = ds18b20_celsius_to_fahrenheit(-55.0f); // DS18B20 min

    TEST_ASSERT_FLOAT_WITHIN(1.0f, 257.0f, very_hot);
    TEST_ASSERT_FLOAT_WITHIN(1.0f, -67.0f, very_cold);

    printf("Very hot (125°C): %.1f°F\n", very_hot);
    printf("Very cold (-55°C): %.1f°F\n", very_cold);
}

/**
 * Test mock GPIO behavior (sanity check for mocking system)
 */
void test_mock_gpio_behavior(void) {
    print_test_banner("Mock GPIO Behavior");

    // Initialize a GPIO pin
    gpio_init(2);
    TEST_ASSERT_EQUAL(1, mock_gpio_get_init_call_count());

    // Set as output and write a value
    gpio_set_dir(2, true);
    TEST_ASSERT_EQUAL(GPIO_OUT, mock_gpio_get_direction(2));

    gpio_put(2, true);
    TEST_ASSERT_TRUE(mock_gpio_get_state(2));

    gpio_put(2, false);
    TEST_ASSERT_FALSE(mock_gpio_get_state(2));

    printf("Mock GPIO tests passed\n");
}

/**
 * Main test runner
 */
int main(void) {
    UNITY_BEGIN();

    test_helpers_init();

    RUN_TEST(test_celsius_to_fahrenheit_conversion);
    RUN_TEST(test_error_strings);
    RUN_TEST(test_temperature_validation);
    RUN_TEST(test_temperature_edge_cases);
    RUN_TEST(test_mock_gpio_behavior);

    test_helpers_cleanup();

    return UNITY_END();
}
