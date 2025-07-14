/**
 * DS18B20 Temperature Sensor Test Program
 * Tests the DS18B20 sensor connected to GPIO 2
 * Pure C implementation for maximum compatibility
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "ds18b20.h"

#define DS18B20_GPIO 2

int main() {
    // Initialize all
    stdio_init_all();
    
    if (cyw43_arch_init()) {
        printf("Failed to initialize CYW43\n");
        return 1;
    }
    
    // Enable onboard LED for status
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    
    printf("DS18B20 Temperature Sensor Test\n");
    printf("================================\n");
    printf("GPIO Pin: %d\n\n", DS18B20_GPIO);
    
    // Initialize DS18B20 sensor
    printf("Initializing DS18B20 sensor...\n");
    ds18b20_result_t result = ds18b20_init(DS18B20_GPIO);
    
    if (result != DS18B20_OK) {
        printf("Failed to initialize DS18B20: %s\n", ds18b20_error_string(result));
        printf("Check connections:\n");
        printf("- Data wire connected to GPIO %d\n", DS18B20_GPIO);
        printf("- 4.7k pull-up resistor between data and 3.3V\n");
        printf("- Power connected to 3.3V and GND\n");
        return 1;
    }
    
    printf("DS18B20 sensor initialized successfully!\n\n");
    
    printf("Starting temperature readings...\n");
    printf("Temperature readings (Ctrl+C to stop):\n");
    printf("Time\t\tTemperature (°C)\tTemperature (°F)\tStatus\n");
    printf("----\t\t----------------\t----------------\t------\n");
    
    int reading_count = 0;
    int error_count = 0;
    absolute_time_t start_time = get_absolute_time();
    
    while (true) {
        // Toggle LED to show activity
        static bool led_state = false;
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_state);
        led_state = !led_state;
        
        // Read temperature
        float temperature_c;
        result = ds18b20_read_temperature(&temperature_c);
        
        // Calculate elapsed time
        int64_t elapsed_us = absolute_time_diff_us(start_time, get_absolute_time());
        int elapsed_seconds = elapsed_us / 1000000;
        
        if (result == DS18B20_OK) {
            float temperature_f = ds18b20_celsius_to_fahrenheit(temperature_c);
            
            printf("%02d:%02d\t\t%.2f°C\t\t\t%.2f°F\t\tOK\n", 
                   elapsed_seconds / 60, elapsed_seconds % 60,
                   temperature_c, temperature_f);
            
            reading_count++;
            
            // Validate temperature range
            if (temperature_c < -55.0f || temperature_c > 125.0f) {
                printf("Warning: Temperature out of range!\n");
            }
        } else {
            printf("%02d:%02d\t\t---.--°C\t\t---.--°F\t\tERROR: %s\n", 
                   elapsed_seconds / 60, elapsed_seconds % 60,
                   ds18b20_error_string(result));
            error_count++;
            
            // If too many errors, suggest checking connections
            if (error_count > 3 && reading_count == 0) {
                printf("\nMultiple errors detected. Please check:\n");
                printf("- DS18B20 is properly connected to GPIO %d\n", DS18B20_GPIO);
                printf("- 4.7k pull-up resistor is installed\n");
                printf("- Power supply is stable\n");
                printf("- Wiring is correct\n\n");
            }
        }
        
        // Wait before next reading
        sleep_ms(2000);
    }
    
    cyw43_arch_deinit();
    return 0;
}
