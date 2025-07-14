/**
 * Version Display Utility Header
 * Common functionality for displaying version information across Pico W applications
 *
 * Copyright (c) 2024 Peter Westlund
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef VERSION_DISPLAY_H
#define VERSION_DISPLAY_H

#include "pico/stdlib.h"

/**
 * Initialize stdio with intelligent USB connection waiting and display version information
 *
 * This function:
 * - Initializes stdio_init_all()
 * - Waits for USB connection (if using USB stdio) with timeout
 * - Displays application version information
 * - Ensures output is flushed and stable
 *
 * @param app_name Name of the application (e.g. "Pico W Home Assistant Sensor")
 * @param timeout_ms Timeout in milliseconds to wait for USB connection (default: 10000ms)
 */
void init_stdio_and_display_version(const char *app_name, uint32_t timeout_ms);

/**
 * Initialize stdio with default timeout (10 seconds)
 *
 * @param app_name Name of the application
 */
static inline void init_stdio_and_display_version_default(const char *app_name) {
    init_stdio_and_display_version(app_name, 10000);
}

#endif // VERSION_DISPLAY_H
