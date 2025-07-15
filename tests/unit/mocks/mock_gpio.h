#ifndef MOCK_GPIO_H
#define MOCK_GPIO_H

/**
 * Mock implementation of Pico SDK GPIO functions for testing on Linux
 */

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Types from Pico SDK
typedef unsigned int uint;
typedef enum { GPIO_IN = 0, GPIO_OUT = 1 } gpio_dir_t;

// Mock control functions
void mock_gpio_reset(void);
bool mock_gpio_get_state(uint gpio);
void mock_gpio_set_state(uint gpio, bool value);
gpio_dir_t mock_gpio_get_direction(uint gpio);
bool mock_gpio_is_pull_up_enabled(uint gpio);

// Call count tracking for verification
int mock_gpio_get_init_call_count(void);
int mock_gpio_get_set_dir_call_count(void);
int mock_gpio_get_put_call_count(void);
int mock_gpio_get_get_call_count(void);

// Mock implementations of Pico SDK functions
void gpio_init(uint gpio);
void gpio_set_dir(uint gpio, bool out);
void gpio_put(uint gpio, bool value);
bool gpio_get(uint gpio);
void gpio_pull_up(uint gpio);
void gpio_pull_down(uint gpio);

#ifdef __cplusplus
}
#endif

#endif // MOCK_GPIO_H
