#include "mock_gpio.h"
#include <stdio.h>
#include <stdbool.h>

// Mock GPIO state
static bool gpio_pins[30] = {false}; // RP2040 has GPIO 0-29
static gpio_dir_t gpio_directions[30] = {GPIO_IN};
static bool gpio_pull_up_enabled[30] = {false};

// Mock statistics
static int gpio_init_call_count = 0;
static int gpio_set_dir_call_count = 0;
static int gpio_put_call_count = 0;
static int gpio_get_call_count = 0;

void mock_gpio_reset(void) {
    // Reset all mock state
    for (int i = 0; i < 30; i++) {
        gpio_pins[i] = false;
        gpio_directions[i] = GPIO_IN;
        gpio_pull_up_enabled[i] = false;
    }

    gpio_init_call_count = 0;
    gpio_set_dir_call_count = 0;
    gpio_put_call_count = 0;
    gpio_get_call_count = 0;

    printf("Mock GPIO reset\n");
}

// Mock implementations of Pico SDK GPIO functions
void gpio_init(uint gpio) {
    if (gpio >= 30)
        return;
    gpio_init_call_count++;
    printf("Mock: gpio_init(%u)\n", gpio);
}

void gpio_set_dir(uint gpio, bool out) {
    if (gpio >= 30)
        return;
    gpio_directions[gpio] = out ? GPIO_OUT : GPIO_IN;
    gpio_set_dir_call_count++;
    printf("Mock: gpio_set_dir(%u, %s)\n", gpio, out ? "OUT" : "IN");
}

void gpio_put(uint gpio, bool value) {
    if (gpio >= 30)
        return;
    if (gpio_directions[gpio] == GPIO_OUT) {
        gpio_pins[gpio] = value;
        gpio_put_call_count++;
        printf("Mock: gpio_put(%u, %s)\n", gpio, value ? "HIGH" : "LOW");
    } else {
        printf("Mock: WARNING - gpio_put() on input pin %u\n", gpio);
    }
}

bool gpio_get(uint gpio) {
    if (gpio >= 30)
        return false;
    gpio_get_call_count++;
    printf("Mock: gpio_get(%u) -> %s\n", gpio, gpio_pins[gpio] ? "HIGH" : "LOW");
    return gpio_pins[gpio];
}

void gpio_pull_up(uint gpio) {
    if (gpio >= 30)
        return;
    gpio_pull_up_enabled[gpio] = true;
    printf("Mock: gpio_pull_up(%u)\n", gpio);
}

void gpio_pull_down(uint gpio) {
    if (gpio >= 30)
        return;
    gpio_pull_up_enabled[gpio] = false;
    printf("Mock: gpio_pull_down(%u)\n", gpio);
}

// Mock state inspection functions
bool mock_gpio_get_state(uint gpio) {
    if (gpio >= 30)
        return false;
    return gpio_pins[gpio];
}

void mock_gpio_set_state(uint gpio, bool value) {
    if (gpio >= 30)
        return;
    gpio_pins[gpio] = value;
}

gpio_dir_t mock_gpio_get_direction(uint gpio) {
    if (gpio >= 30)
        return GPIO_IN;
    return gpio_directions[gpio];
}

bool mock_gpio_is_pull_up_enabled(uint gpio) {
    if (gpio >= 30)
        return false;
    return gpio_pull_up_enabled[gpio];
}

int mock_gpio_get_init_call_count(void) {
    return gpio_init_call_count;
}

int mock_gpio_get_set_dir_call_count(void) {
    return gpio_set_dir_call_count;
}

int mock_gpio_get_put_call_count(void) {
    return gpio_put_call_count;
}

int mock_gpio_get_get_call_count(void) {
    return gpio_get_call_count;
}
