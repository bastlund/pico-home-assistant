#include "unity.h"
#include "test_helpers.h"
#include "mock_gpio.h"
#include "mock_pico.h"

// Integration test that tests multiple components together
void setUp(void) {
    mock_gpio_reset();
    mock_pico_reset();
    test_helpers_init();
}

void tearDown(void) {
    test_helpers_cleanup();
}

/**
 * Test full system initialization simulation
 */
void test_system_initialization(void) {
    print_test_banner("System Initialization");

    // Simulate system startup sequence
    stdio_init_all();

    // Initialize DS18B20 GPIO
    const uint ds18b20_pin = 2;
    gpio_init(ds18b20_pin);
    gpio_set_dir(ds18b20_pin, false); // Input
    gpio_pull_up(ds18b20_pin);

    // Verify initialization
    TEST_ASSERT_EQUAL(GPIO_IN, mock_gpio_get_direction(ds18b20_pin));
    TEST_ASSERT_TRUE(mock_gpio_is_pull_up_enabled(ds18b20_pin));
    TEST_ASSERT_EQUAL(1, mock_gpio_get_init_call_count());

    printf("System initialization completed successfully\n");
}

/**
 * Test sensor reading simulation with mocked hardware
 */
void test_sensor_reading_simulation(void) {
    print_test_banner("Sensor Reading Simulation");

    const uint sensor_pin = 2;

    // Initialize sensor
    gpio_init(sensor_pin);
    gpio_pull_up(sensor_pin);

    // Simulate sensor communication protocol
    // 1. Reset pulse (output low)
    gpio_set_dir(sensor_pin, true);
    gpio_put(sensor_pin, false);
    sleep_us(480); // Reset pulse duration

    // 2. Release bus (input)
    gpio_set_dir(sensor_pin, false);
    sleep_us(70); // Wait for presence pulse

    // 3. Check for presence pulse (simulate device present)
    mock_gpio_set_state(sensor_pin, false); // Device pulls low
    bool presence = !gpio_get(sensor_pin);
    TEST_ASSERT_TRUE(presence);

    sleep_us(410); // Wait for presence pulse to finish

    // 4. Verify mock tracking
    TEST_ASSERT_GREATER_THAN(0, mock_gpio_get_set_dir_call_count());
    TEST_ASSERT_GREATER_THAN(0, mock_gpio_get_put_call_count());
    TEST_ASSERT_GREATER_THAN(0, mock_gpio_get_get_call_count());

    printf("Sensor reading simulation completed\n");
}

/**
 * Test error handling scenarios
 */
void test_error_handling_scenarios(void) {
    print_test_banner("Error Handling Scenarios");

    const uint sensor_pin = 2;

    // Test scenario 1: No device present
    gpio_init(sensor_pin);
    gpio_set_dir(sensor_pin, true);
    gpio_put(sensor_pin, false);
    sleep_us(480);

    gpio_set_dir(sensor_pin, false);
    sleep_us(70);

    // Simulate no device (bus stays high)
    mock_gpio_set_state(sensor_pin, true);
    bool no_presence = gpio_get(sensor_pin);
    TEST_ASSERT_TRUE(no_presence); // No device detected

    printf("No device scenario: PASSED\n");

    // Test scenario 2: CRC error simulation
    // This would involve more complex data validation
    uint8_t mock_data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0xFF}; // Bad CRC
    uint8_t crc = 0;
    for (int i = 0; i < 8; i++) {
        // Simple XOR "CRC" for demonstration
        crc ^= mock_data[i];
    }

    bool crc_valid = (crc == mock_data[8]);
    TEST_ASSERT_FALSE(crc_valid); // Should detect CRC error

    printf("CRC error scenario: PASSED\n");
}

/**
 * Test timing requirements
 */
void test_timing_requirements(void) {
    print_test_banner("Timing Requirements");

    // Test that our timing functions work as expected
    absolute_time_t start = get_absolute_time();

    // Simulate time-critical operations
    sleep_us(1);   // Minimum delay
    sleep_us(15);  // Typical 1-Wire timing
    sleep_us(60);  // Maximum timing window
    sleep_us(480); // Reset timing

    absolute_time_t end = get_absolute_time();
    int64_t total_time = absolute_time_diff_us(start, end);

    // Should be at least the sum of all delays (556us)
    TEST_ASSERT_GREATER_OR_EQUAL(556, total_time);

    printf("Total timing test duration: %lld us\n", (long long) total_time);
}

/**
 * Test system cleanup and resource management
 */
void test_system_cleanup(void) {
    print_test_banner("System Cleanup");

    // Initialize multiple resources
    const uint pins[] = {2, 3, 4};
    const int num_pins = sizeof(pins) / sizeof(pins[0]);

    for (int i = 0; i < num_pins; i++) {
        gpio_init(pins[i]);
        gpio_set_dir(pins[i], false);
        gpio_pull_up(pins[i]);
    }

    int init_calls = mock_gpio_get_init_call_count();
    TEST_ASSERT_EQUAL(num_pins, init_calls);

    // Simulate cleanup (reset all pins to safe state)
    for (int i = 0; i < num_pins; i++) {
        gpio_set_dir(pins[i], false);        // Input
        mock_gpio_set_state(pins[i], false); // Low
    }

    // Verify cleanup
    for (int i = 0; i < num_pins; i++) {
        TEST_ASSERT_EQUAL(GPIO_IN, mock_gpio_get_direction(pins[i]));
    }

    printf("System cleanup completed for %d pins\n", num_pins);
}

/**
 * Main integration test runner
 */
int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_system_initialization);
    RUN_TEST(test_sensor_reading_simulation);
    RUN_TEST(test_error_handling_scenarios);
    RUN_TEST(test_timing_requirements);
    RUN_TEST(test_system_cleanup);

    return UNITY_END();
}
