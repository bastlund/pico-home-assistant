/**
 * Pico W Home Assistant Sensor
 * Based on Raspberry Pi Pico examples
 * 
 * Copyright (c) 2024 Peter Westlund
 * Original Pico examples: Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/unique_id.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/adc.h"
#include "lwip/apps/mqtt.h"
#include "lwip/apps/mqtt_priv.h" // needed to set hostname
#include "lwip/dns.h"
#include "lwip/altcp_tls.h"
#include <math.h> // for fabs

// Temperature
#ifndef TEMPERATURE_UNITS
#define TEMPERATURE_UNITS 'C' // Set to 'F' for Fahrenheit
#endif

// Home Assistant MQTT Discovery
#ifndef HA_DISCOVERY_PREFIX
#define HA_DISCOVERY_PREFIX "homeassistant"
#endif

#ifndef HA_DEVICE_NAME
#define HA_DEVICE_NAME "Pico W Sensor"
#endif

#ifndef HA_DEVICE_MODEL
#define HA_DEVICE_MODEL "Raspberry Pi Pico W"
#endif

#ifndef HA_DEVICE_MANUFACTURER
#define HA_DEVICE_MANUFACTURER "Raspberry Pi Foundation"
#endif

#ifndef MQTT_SERVER
#error Need to define MQTT_SERVER
#endif

// MQTT port configuration
#ifndef MQTT_PORT
#define MQTT_PORT 1883
#endif

#ifndef MQTT_TLS_PORT
#define MQTT_TLS_PORT 8883
#endif

// This file includes your client certificate for client server authentication
#ifdef MQTT_CERT_INC
#include MQTT_CERT_INC
#endif

#ifndef MQTT_TOPIC_LEN
#define MQTT_TOPIC_LEN 200  // Increased for HA discovery topics
#endif

#ifndef MQTT_CONFIG_LEN
#define MQTT_CONFIG_LEN 1000  // For HA discovery config payloads
#endif

typedef struct {
    mqtt_client_t* mqtt_client_inst;
    struct mqtt_connect_client_info_t mqtt_client_info;
    char data[MQTT_OUTPUT_RINGBUF_SIZE];
    char topic[MQTT_TOPIC_LEN];
    uint32_t len;
    ip_addr_t mqtt_server_address;
    bool connect_done;
    int subscribe_count;
    bool stop_client;
    char device_id[16];  // Unique device identifier
    bool ha_discovery_sent;  // Track if HA discovery has been sent
    absolute_time_t discovery_send_time;  // When to send discovery
} MQTT_CLIENT_DATA_T;

// Debug level configuration
// 0 = No debug output (ERRORS only)
// 1 = ERROR + WARN
// 2 = ERROR + WARN + INFO  
// 3 = ERROR + WARN + INFO + DEBUG
// 4 = ERROR + WARN + INFO + DEBUG + VERBOSE
#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 2  // Default to INFO level
#endif

// Debug macros based on debug level
#ifndef ERROR_printf
#define ERROR_printf printf  // Always enabled
#endif

#ifndef WARN_printf
#if DEBUG_LEVEL >= 1
#define WARN_printf printf
#else
#define WARN_printf(...)
#endif
#endif

#ifndef INFO_printf
#if DEBUG_LEVEL >= 2
#define INFO_printf printf
#else
#define INFO_printf(...)
#endif
#endif

#ifndef DEBUG_printf
#if DEBUG_LEVEL >= 3
#define DEBUG_printf printf
#else
#define DEBUG_printf(...)
#endif
#endif

#ifndef VERBOSE_printf
#if DEBUG_LEVEL >= 4
#define VERBOSE_printf printf
#else
#define VERBOSE_printf(...)
#endif
#endif

// how often to measure our temperature
#define TEMP_WORKER_TIME_S 10

// keep alive in seconds - reduced for better detection of connection issues
#define MQTT_KEEP_ALIVE_S 30

// qos passed to mqtt_subscribe
// At most once (QoS 0)
// At least once (QoS 1)
// Exactly once (QoS 2)
#define MQTT_SUBSCRIBE_QOS 1
#define MQTT_PUBLISH_QOS 1
#define MQTT_PUBLISH_RETAIN 0

// topic used for last will and testament
#define MQTT_WILL_TOPIC "/online"
#define MQTT_WILL_MSG "0"
#define MQTT_WILL_QOS 1

#ifndef MQTT_DEVICE_NAME
#define MQTT_DEVICE_NAME "pico"
#endif

// Set to 1 to add the client name to topics, to support multiple devices using the same server
#ifndef MQTT_UNIQUE_TOPIC
#define MQTT_UNIQUE_TOPIC 0
#endif

/* References for this implementation:
 * raspberry-pi-pico-c-sdk.pdf, Section '4.1.1. hardware_adc'
 * pico-examples/adc/adc_console/adc_console.c */
static float read_onboard_temperature(const char unit) {

    /* 12-bit conversion, assume max value == ADC_VREF == 3.3 V */
    const float conversionFactor = 3.3f / (1 << 12);

    float adc = (float)adc_read() * conversionFactor;
    float tempC = 27.0f - (adc - 0.706f) / 0.001721f;

    if (unit == 'C' || unit != 'F') {
        return tempC;
    } else if (unit == 'F') {
        return tempC * 9 / 5 + 32;
    }

    return -1.0f;
}

static void pub_request_cb(__unused void *arg, err_t err) {
    if (err != 0) {
        ERROR_printf("MQTT publish callback failed with error %d\n", err);
        switch(err) {
            case ERR_MEM:
                ERROR_printf("  -> Out of memory\n");
                break;
            case ERR_BUF:
                ERROR_printf("  -> Buffer error\n");
                break;
            case ERR_TIMEOUT:
                ERROR_printf("  -> Timeout\n");
                break;
            case ERR_CONN:
                ERROR_printf("  -> Connection error\n");
                break;
            default:
                ERROR_printf("  -> Unknown error\n");
                break;
        }
    } else {
        INFO_printf("MQTT publish successful\n");
    }
}

// Home Assistant MQTT Discovery functions
static void publish_ha_discovery(MQTT_CLIENT_DATA_T *state) {
    if (state->ha_discovery_sent) {
        INFO_printf("HA Discovery already sent, skipping\n");
        return; // Already sent
    }

    char config_topic[MQTT_TOPIC_LEN];
    char config_payload[MQTT_CONFIG_LEN];
    char state_topic[MQTT_TOPIC_LEN];
    char availability_topic[MQTT_TOPIC_LEN];
    
    // Temperature sensor discovery
    snprintf(config_topic, sizeof(config_topic), 
             "%s/sensor/%s/temperature/config", HA_DISCOVERY_PREFIX, state->device_id);
    
    snprintf(state_topic, sizeof(state_topic), 
             "pico/%s/temperature", state->device_id);
             
    snprintf(availability_topic, sizeof(availability_topic), 
             "pico/%s/status", state->device_id);

    snprintf(config_payload, sizeof(config_payload),
        "{"
        "\"name\":\"%s Temperature\","
        "\"device_class\":\"temperature\","
        "\"state_topic\":\"%s\","
        "\"availability_topic\":\"%s\","
        "\"payload_available\":\"online\","
        "\"payload_not_available\":\"offline\","
        "\"unit_of_measurement\":\"%s\","
        "\"value_template\":\"{{ value_json.temperature }}\","
        "\"unique_id\":\"%s_temperature\","
        "\"device\":{"
            "\"identifiers\":[\"%s\"],"
            "\"name\":\"%s\","
            "\"model\":\"%s\","
            "\"manufacturer\":\"%s\","
            "\"sw_version\":\"1.0\""
        "},"
        "\"expire_after\":300"
        "}",
        HA_DEVICE_NAME,
        state_topic,
        availability_topic,
        (TEMPERATURE_UNITS == 'F') ? "°F" : "°C",
        state->device_id,
        state->device_id,
        HA_DEVICE_NAME,
        HA_DEVICE_MODEL,
        HA_DEVICE_MANUFACTURER
    );

    INFO_printf("Publishing HA discovery config:\n");
    INFO_printf("Topic: %s\n", config_topic);
    INFO_printf("Payload length: %d\n", strlen(config_payload));
    
    err_t result = mqtt_publish(state->mqtt_client_inst, config_topic, config_payload, 
                strlen(config_payload), MQTT_PUBLISH_QOS, true, pub_request_cb, state);
    
    if (result == ERR_OK) {
        INFO_printf("HA Discovery config published successfully\n");
        state->ha_discovery_sent = true;
    } else {
        ERROR_printf("Failed to publish HA discovery config, error: %d\n", result);
    }
}

static void publish_ha_availability(MQTT_CLIENT_DATA_T *state, bool online) {
    char availability_topic[MQTT_TOPIC_LEN];
    snprintf(availability_topic, sizeof(availability_topic), 
             "pico/%s/status", state->device_id);
    
    const char* status = online ? "online" : "offline";
    INFO_printf("Publishing availability: %s to %s\n", status, availability_topic);
    
    // Don't use lwip locking here since we're already in a callback
    err_t result = mqtt_publish(state->mqtt_client_inst, availability_topic, status, 
                strlen(status), MQTT_PUBLISH_QOS, true, pub_request_cb, state);
    
    if (result != ERR_OK) {
        ERROR_printf("Failed to publish availability, error: %d\n", result);
    } else {
        INFO_printf("Availability published successfully\n");
    }
}

static const char *full_topic(MQTT_CLIENT_DATA_T *state, const char *name) {
#if MQTT_UNIQUE_TOPIC
    static char full_topic[MQTT_TOPIC_LEN];
    snprintf(full_topic, sizeof(full_topic), "/%s%s", state->mqtt_client_info.client_id, name);
    return full_topic;
#else
    return name;
#endif
}

static void control_led(MQTT_CLIENT_DATA_T *state, bool on) {
    // Publish state on /state topic and on/off led board
    const char* message = on ? "On" : "Off";
    if (on)
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    else
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);

    mqtt_publish(state->mqtt_client_inst, full_topic(state, "/led/state"), message, strlen(message), MQTT_PUBLISH_QOS, MQTT_PUBLISH_RETAIN, pub_request_cb, state);
}

static void publish_temperature(MQTT_CLIENT_DATA_T *state) {
    static float old_temperature = -999.0; // Initialize with unlikely value
    float temperature = read_onboard_temperature(TEMPERATURE_UNITS);
    
    VERBOSE_printf("Raw temperature reading: %.2f°%c (old: %.2f°%c)\n", 
                   temperature, TEMPERATURE_UNITS, old_temperature, TEMPERATURE_UNITS);
    
    // Only publish if temperature has changed significantly (0.1 degree threshold)
    if (fabs(temperature - old_temperature) > 0.1) {
        old_temperature = temperature;
        
        // Create Home Assistant compatible topic
        char temperature_topic[MQTT_TOPIC_LEN];
        snprintf(temperature_topic, sizeof(temperature_topic), 
                 "pico/%s/temperature", state->device_id);
        
        // Create JSON payload for Home Assistant
        char temp_payload[100];
        snprintf(temp_payload, sizeof(temp_payload), 
                 "{\"temperature\":%.2f}", temperature);
        
        DEBUG_printf("Temperature payload: %s\n", temp_payload);
        INFO_printf("Publishing temperature %.2f°%c to %s\n", 
                   temperature, TEMPERATURE_UNITS, temperature_topic);
        
        err_t result = mqtt_publish(state->mqtt_client_inst, temperature_topic, temp_payload, 
                    strlen(temp_payload), MQTT_PUBLISH_QOS, MQTT_PUBLISH_RETAIN, 
                    pub_request_cb, state);
        
        if (result != ERR_OK) {
            ERROR_printf("Failed to publish temperature, error: %d\n", result);
        } else {
            INFO_printf("Temperature published successfully\n");
        }
    }
}

static void sub_request_cb(void *arg, err_t err) {
    MQTT_CLIENT_DATA_T* state = (MQTT_CLIENT_DATA_T*)arg;
    if (err != 0) {
        panic("subscribe request failed %d", err);
    }
    state->subscribe_count++;
}

static void unsub_request_cb(void *arg, err_t err) {
    MQTT_CLIENT_DATA_T* state = (MQTT_CLIENT_DATA_T*)arg;
    if (err != 0) {
        panic("unsubscribe request failed %d", err);
    }
    state->subscribe_count--;
    assert(state->subscribe_count >= 0);

    // Stop if requested
    if (state->subscribe_count <= 0 && state->stop_client) {
        mqtt_disconnect(state->mqtt_client_inst);
    }
}

static void sub_unsub_topics(MQTT_CLIENT_DATA_T* state, bool sub) {
    mqtt_request_cb_t cb = sub ? sub_request_cb : unsub_request_cb;
    mqtt_sub_unsub(state->mqtt_client_inst, full_topic(state, "/led"), MQTT_SUBSCRIBE_QOS, cb, state, sub);
    mqtt_sub_unsub(state->mqtt_client_inst, full_topic(state, "/print"), MQTT_SUBSCRIBE_QOS, cb, state, sub);
    mqtt_sub_unsub(state->mqtt_client_inst, full_topic(state, "/ping"), MQTT_SUBSCRIBE_QOS, cb, state, sub);
    mqtt_sub_unsub(state->mqtt_client_inst, full_topic(state, "/exit"), MQTT_SUBSCRIBE_QOS, cb, state, sub);
}

static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags) {
    MQTT_CLIENT_DATA_T* state = (MQTT_CLIENT_DATA_T*)arg;
#if MQTT_UNIQUE_TOPIC
    const char *basic_topic = state->topic + strlen(state->mqtt_client_info.client_id) + 1;
#else
    const char *basic_topic = state->topic;
#endif
    strncpy(state->data, (const char *)data, len);
    state->len = len;
    state->data[len] = '\0';

    VERBOSE_printf("Raw MQTT data received: topic=%s, len=%d, flags=0x%x\n", state->topic, len, flags);
    DEBUG_printf("Topic: %s, Message: %s\n", state->topic, state->data);
    if (strcmp(basic_topic, "/led") == 0)
    {
        if (lwip_stricmp((const char *)state->data, "On") == 0 || strcmp((const char *)state->data, "1") == 0)
            control_led(state, true);
        else if (lwip_stricmp((const char *)state->data, "Off") == 0 || strcmp((const char *)state->data, "0") == 0)
            control_led(state, false);
    } else if (strcmp(basic_topic, "/print") == 0) {
        INFO_printf("%.*s\n", len, data);
    } else if (strcmp(basic_topic, "/ping") == 0) {
        char buf[11];
        snprintf(buf, sizeof(buf), "%u", to_ms_since_boot(get_absolute_time()) / 1000);
        mqtt_publish(state->mqtt_client_inst, full_topic(state, "/uptime"), buf, strlen(buf), MQTT_PUBLISH_QOS, MQTT_PUBLISH_RETAIN, pub_request_cb, state);
    } else if (strcmp(basic_topic, "/exit") == 0) {
        state->stop_client = true; // stop the client when ALL subscriptions are stopped
        sub_unsub_topics(state, false); // unsubscribe
    }
}

static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len) {
    MQTT_CLIENT_DATA_T* state = (MQTT_CLIENT_DATA_T*)arg;
    strncpy(state->topic, topic, sizeof(state->topic));
}

static void temperature_worker_fn(async_context_t *context, async_at_time_worker_t *worker) {
    MQTT_CLIENT_DATA_T* state = (MQTT_CLIENT_DATA_T*)worker->user_data;
    static int publish_step = 0;
    
    // Check if MQTT is still connected
    if (!mqtt_client_is_connected(state->mqtt_client_inst)) {
        ERROR_printf("MQTT connection lost! Attempting to reconnect...\n");
        // Reset publish step to restart sequence after reconnection
        publish_step = 0;
        async_context_add_at_time_worker_in_ms(context, worker, 5000); // Try again in 5 seconds
        return;
    }
    
    switch(publish_step) {
        case 0:
            // First run - publish availability
            INFO_printf("Step 1: Publishing availability\n");
            publish_ha_availability(state, true);
            publish_step++;
            async_context_add_at_time_worker_in_ms(context, worker, 2000); // Wait 2 seconds
            break;
            
        case 1:
            // Second run - publish discovery
            INFO_printf("Step 2: Publishing HA discovery\n");
            publish_ha_discovery(state);
            publish_step++;
            async_context_add_at_time_worker_in_ms(context, worker, 2000); // Wait 2 seconds
            break;
            
        case 2:
            // Third run - publish initial temperature
            INFO_printf("Step 3: Publishing initial temperature\n");
            publish_temperature(state);
            publish_step++;
            async_context_add_at_time_worker_in_ms(context, worker, TEMP_WORKER_TIME_S * 1000);
            break;
            
        default:
            // Normal operation - just publish temperature
            INFO_printf("Normal operation: Publishing temperature\n");
            publish_temperature(state);
            async_context_add_at_time_worker_in_ms(context, worker, TEMP_WORKER_TIME_S * 1000);
            break;
    }
}
static async_at_time_worker_t temperature_worker = { .do_work = temperature_worker_fn };

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    MQTT_CLIENT_DATA_T* state = (MQTT_CLIENT_DATA_T*)arg;
    if (status == MQTT_CONNECT_ACCEPTED) {
        state->connect_done = true;
        INFO_printf("MQTT connected successfully!\n");
        
        // Reset discovery flag for reconnection
        state->ha_discovery_sent = false;
        
        // Subscribe to topics first
        sub_unsub_topics(state, true);

        // Set up discovery timing - send after 5 seconds to allow things to settle
        state->discovery_send_time = make_timeout_time_ms(5000);

        // Start temperature worker - this will handle all publishing
        temperature_worker.user_data = state;
        async_context_add_at_time_worker_in_ms(cyw43_arch_async_context(), &temperature_worker, 1000); // Start after 1 second
        
        INFO_printf("MQTT setup complete, publishing will start shortly\n");
    } else if (status == MQTT_CONNECT_DISCONNECTED) {
        ERROR_printf("MQTT disconnected!\n");
        state->connect_done = false;
        
        // Check WiFi status
        int wifi_status = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
        INFO_printf("WiFi link status: %d\n", wifi_status);
        
        if (wifi_status != CYW43_LINK_UP) {
            ERROR_printf("WiFi connection lost! Attempting to reconnect...\n");
            // Try to reconnect WiFi
            if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000) == 0) {
                INFO_printf("WiFi reconnected successfully\n");
                // MQTT will try to reconnect automatically
            } else {
                ERROR_printf("Failed to reconnect WiFi\n");
            }
        }
    }
    else {
        // Print the status code for debugging
        ERROR_printf("MQTT connection failed with status %d\n", status);
    }
}

static void start_client(MQTT_CLIENT_DATA_T *state) {
#if LWIP_ALTCP && LWIP_ALTCP_TLS
    const int port = MQTT_TLS_PORT;
    INFO_printf("Using TLS\n");
#else
    const int port = MQTT_PORT;
    INFO_printf("Warning: Not using TLS\n");
#endif

    state->mqtt_client_inst = mqtt_client_new();
    if (!state->mqtt_client_inst) {
        panic("MQTT client instance creation error");
    }
    INFO_printf("IP address of this device %s\n", ipaddr_ntoa(&(netif_list->ip_addr)));
    INFO_printf("Connecting to mqtt server at %s\n", ipaddr_ntoa(&state->mqtt_server_address));

    cyw43_arch_lwip_begin();
    if (mqtt_client_connect(state->mqtt_client_inst, &state->mqtt_server_address, port, mqtt_connection_cb, state, &state->mqtt_client_info) != ERR_OK) {
        panic("MQTT broker connection error");
    }
#if LWIP_ALTCP && LWIP_ALTCP_TLS
    // This is important for MBEDTLS_SSL_SERVER_NAME_INDICATION
    mbedtls_ssl_set_hostname(altcp_tls_context(state->mqtt_client_inst->conn), MQTT_SERVER);
#endif
    mqtt_set_inpub_callback(state->mqtt_client_inst, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, state);
    cyw43_arch_lwip_end();
}

// Call back with a DNS result
static void dns_found(const char *hostname, const ip_addr_t *ipaddr, void *arg) {
    MQTT_CLIENT_DATA_T *state = (MQTT_CLIENT_DATA_T*)arg;
    if (ipaddr) {
        state->mqtt_server_address = *ipaddr;
        start_client(state);
    } else {
        panic("dns request failed");
    }
}

int main(void) {
    stdio_init_all();
    INFO_printf("mqtt client starting\n");

    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);

    static MQTT_CLIENT_DATA_T state;

    if (cyw43_arch_init()) {
        panic("Failed to inizialize CYW43");
    }

    // Use board unique id
    char unique_id_buf[5];
    pico_get_unique_board_id_string(unique_id_buf, sizeof(unique_id_buf));
    for(int i=0; i < sizeof(unique_id_buf) - 1; i++) {
        unique_id_buf[i] = tolower(unique_id_buf[i]);
    }

    // Generate a unique name, e.g. pico1234
    char client_id_buf[sizeof(MQTT_DEVICE_NAME) + sizeof(unique_id_buf) - 1];
    memcpy(&client_id_buf[0], MQTT_DEVICE_NAME, sizeof(MQTT_DEVICE_NAME) - 1);
    memcpy(&client_id_buf[sizeof(MQTT_DEVICE_NAME) - 1], unique_id_buf, sizeof(unique_id_buf) - 1);
    client_id_buf[sizeof(client_id_buf) - 1] = 0;
    INFO_printf("Device name %s\n", client_id_buf);

    // Set device ID for Home Assistant
    strncpy(state.device_id, client_id_buf, sizeof(state.device_id) - 1);
    state.device_id[sizeof(state.device_id) - 1] = 0;
    state.ha_discovery_sent = false;
    state.discovery_send_time = nil_time;

    state.mqtt_client_info.client_id = client_id_buf;
    state.mqtt_client_info.keep_alive = MQTT_KEEP_ALIVE_S; // Keep alive in sec
#if defined(MQTT_USERNAME) && defined(MQTT_PASSWORD)
    state.mqtt_client_info.client_user = MQTT_USERNAME;
    state.mqtt_client_info.client_pass = MQTT_PASSWORD;
#else
    state.mqtt_client_info.client_user = NULL;
    state.mqtt_client_info.client_pass = NULL;
#endif
    static char will_topic[MQTT_TOPIC_LEN];
    snprintf(will_topic, sizeof(will_topic), "pico/%s/status", state.device_id);
    state.mqtt_client_info.will_topic = will_topic;
    state.mqtt_client_info.will_msg = "offline";
    state.mqtt_client_info.will_qos = MQTT_WILL_QOS;
    state.mqtt_client_info.will_retain = true;
#if LWIP_ALTCP && LWIP_ALTCP_TLS
    // TLS enabled
#ifdef MQTT_CERT_INC
    static const uint8_t ca_cert[] = TLS_ROOT_CERT;
    static const uint8_t client_key[] = TLS_CLIENT_KEY;
    static const uint8_t client_cert[] = TLS_CLIENT_CERT;
    // This confirms the indentity of the server and the client
    state.mqtt_client_info.tls_config = altcp_tls_create_config_client_2wayauth(ca_cert, sizeof(ca_cert),
            client_key, sizeof(client_key), NULL, 0, client_cert, sizeof(client_cert));
#if ALTCP_MBEDTLS_AUTHMODE != MBEDTLS_SSL_VERIFY_REQUIRED
    WARN_printf("Warning: tls without verification is insecure\n");
#endif
#else
    state.mqtt_client_info.tls_config = altcp_tls_create_config_client(NULL, 0);
    WARN_printf("Warning: tls without a certificate is insecure\n");
#endif
#endif

    cyw43_arch_enable_sta_mode();
    
    // Disable WiFi power management to improve stability
    cyw43_wifi_pm(&cyw43_state, CYW43_NO_POWERSAVE_MODE);
    INFO_printf("WiFi power management disabled for better stability\n");
    
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        panic("Failed to connect");
    }
    INFO_printf("\nConnected to Wifi\n");

    // We are not in a callback so locking is needed when calling lwip
    // Make a DNS request for the MQTT server IP address
    cyw43_arch_lwip_begin();
    int err = dns_gethostbyname(MQTT_SERVER, &state.mqtt_server_address, dns_found, &state);
    cyw43_arch_lwip_end();

    if (err == ERR_OK) {
        // We have the address, just start the client
        start_client(&state);
    } else if (err != ERR_INPROGRESS) { // ERR_INPROGRESS means expect a callback
        panic("dns request failed");
    }

    while (true) {
        cyw43_arch_poll();
        
        // Check WiFi status periodically
        static absolute_time_t last_wifi_check = {0};
        if (absolute_time_diff_us(last_wifi_check, get_absolute_time()) > 30000000) { // Check every 30 seconds
            last_wifi_check = get_absolute_time();
            int wifi_status = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
            
            VERBOSE_printf("Periodic WiFi status check: status=%d\n", wifi_status);
            
            // Get signal strength
            int32_t rssi = 0;
            if (wifi_status == CYW43_LINK_UP) {
                cyw43_wifi_get_rssi(&cyw43_state, &rssi);
                INFO_printf("WiFi RSSI: %d dBm\n", rssi);
                
                // Warn if signal is weak (below -70 dBm is considered weak)
                if (rssi < -70) {
                    WARN_printf("Weak WiFi signal detected: %d dBm\n", rssi);
                }
            } else {
                ERROR_printf("WiFi connection lost! Status: %d\n", wifi_status);
                // Try immediate WiFi reconnection
                ERROR_printf("Attempting WiFi reconnection...\n");
                if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000) == 0) {
                    INFO_printf("WiFi reconnected successfully\n");
                } else {
                    ERROR_printf("Failed to reconnect WiFi\n");
                }
            }
        }
        
        // If MQTT disconnected, try to reconnect
        if (state.connect_done && !mqtt_client_is_connected(state.mqtt_client_inst)) {
            static absolute_time_t last_reconnect_attempt = {0};
            if (absolute_time_diff_us(last_reconnect_attempt, get_absolute_time()) > 10000000) { // Try every 10 seconds
                last_reconnect_attempt = get_absolute_time();
                ERROR_printf("Attempting MQTT reconnection...\n");
                state.connect_done = false;
                start_client(&state);
            }
        }
        
        cyw43_arch_wait_for_work_until(make_timeout_time_ms(1000));
    }
}
