/**
 * Version Display Utility Implementation
 * Common functionality for displaying version information across Pico W applications
 * 
 * Copyright (c) 2024 Peter Westlund
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "version_display.h"
#include <stdio.h>
#include <string.h>

/**
 * @file version_display.c
 * @brief This module contains functions to set up standard I/O for the device and diplay
 * * version information.
 * 
 * @param app_name Name of the application (e.g. "Pico W Home Assistant Sensor")
 * @param timeout_ms Timeout in milliseconds to wait for USB connection
 */
void init_stdio_and_display_version(const char* app_name, uint32_t timeout_ms) {
    // Initialize stdio
    stdio_init_all();

    /* Wait for USB connection (if using USB stdio) or timeout */
    absolute_time_t timeout = make_timeout_time_ms(timeout_ms);
    while (!stdio_usb_connected() && !time_reached(timeout)) {
        sleep_ms(100);
    }
    
    /* Additional small delay to ensure stability */
    sleep_ms(100);
    
    /* Display version information */
    printf("=== %s ===\n", app_name);
    printf("Version: %s\n", PROJECT_VERSION_FULL);
    printf("Base version: %s\n", PROJECT_VERSION);
    printf("Build info: Major=%d, Minor=%d, Patch=%d\n", 
           PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR, PROJECT_VERSION_PATCH);
    
    /* Print separator line with same length as app name + 8 (for "=== " and " ===") */
    int separator_length = strlen(app_name) + 8;
    for (int i = 0; i < separator_length; i++) {
        printf("=");
    }
    printf("\n");
    
    /* Flush output to ensure it's displayed */
    fflush(stdout);
}
