/**
 * Pico W WiFi Network Scanner
 * Based on Raspberry Pi Pico examples
 * 
 * Copyright (c) 2024 Peter Westlund
 * Original Pico examples: Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* Standard library includes */
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/vreg.h"
#include "hardware/clocks.h"
#include "version_display.h"

/**
 * Callback function for WiFi scan results
 * 
 * @param env Environment pointer (unused)
 * @param result Scan result structure
 * @return 0 to continue scanning
 */
static int scan_result(void *env, const cyw43_ev_scan_result_t *result)
{
    if (result) {
        printf("ssid: %-32s rssi: %4d chan: %3d mac: %02x:%02x:%02x:%02x:%02x:%02x sec: %u\n",
            result->ssid, result->rssi, result->channel,
            result->bssid[0], result->bssid[1], result->bssid[2], 
            result->bssid[3], result->bssid[4], result->bssid[5],
            result->auth_mode);
    }
    return 0;
}

/**
 * Main function - WiFi scanner application
 * 
 * @return Exit status code
 */
int main(void)
{
    /* Initialize stdio and display version information */
    init_stdio_and_display_version_default("Pico W WiFi Network Scanner");

    if (cyw43_arch_init()) {
        printf("cyw43 failed to init\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();

    absolute_time_t scan_time = nil_time;
    bool scan_in_progress = false;
    while(true) {
        if (absolute_time_diff_us(get_absolute_time(), scan_time) < 0) {
            if (!scan_in_progress) {
                cyw43_wifi_scan_options_t scan_options = {0};
                int err = cyw43_wifi_scan(&cyw43_state, &scan_options, NULL, scan_result);
                if (err == 0) {
                    printf("\nPerforming wifi scan\n");
                    scan_in_progress = true;
                } else {
                    printf("Failed to start scan: %d\n", err);
                    scan_time = make_timeout_time_ms(10000); 
                }
            } else if (!cyw43_wifi_scan_active(&cyw43_state)) {
                scan_time = make_timeout_time_ms(10000); 
                scan_in_progress = false; 
            }
        }
        
        sleep_ms(1000);
    }

    cyw43_arch_deinit();
    return 0;
}
