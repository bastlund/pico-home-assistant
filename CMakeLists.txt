cmake_minimum_required(VERSION 3.13)
include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

# Project version
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

project(pico_w VERSION ${PROJECT_VERSION} LANGUAGES C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# Add version information as compile definitions
add_compile_definitions(
    PROJECT_VERSION="${PROJECT_VERSION}"
    PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    PROJECT_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

# Scan application
add_executable(pico_w_scan ping.c)
target_include_directories(pico_w_scan PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(pico_w_scan pico_stdlib pico_cyw43_arch_lwip_threadsafe_background)
pico_add_extra_outputs(pico_w_scan)
pico_enable_stdio_usb(pico_w_scan 1)
pico_enable_stdio_uart(pico_w_scan 0)

# Ping application
add_executable(pico_w_ping ping.c)
target_include_directories(pico_w_ping PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(pico_w_ping pico_stdlib pico_cyw43_arch_lwip_threadsafe_background)
pico_add_extra_outputs(pico_w_ping)
pico_enable_stdio_usb(pico_w_ping 1)
pico_enable_stdio_uart(pico_w_ping 0)

# PingSensor application
add_executable(pico_w_sensor sensor.c)
target_include_directories(pico_w_sensor PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(pico_w_sensor
    pico_stdlib
    hardware_adc
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_mqtt
    pico_mbedtls
    pico_lwip_mbedtls
)
pico_add_extra_outputs(pico_w_sensor)
pico_enable_stdio_usb(pico_w_sensor 1)
pico_enable_stdio_uart(pico_w_sensor 0)

# Define the WiFi credentials
if(NOT DEFINED WIFI_SSID)
    message(FATAL_ERROR "WIFI credentials missing: WIFI_SSID is not defined. Please define it via -DWIFI_SSID=\"YourSSID\" when running cmake, or set it in CMakeLists.txt.")
endif()

if(NOT DEFINED WIFI_PASSWORD)
    message(FATAL_ERROR "WIFI credentials missing: WIFI_PASSWORD is not defined. Please define it via -DWIFI_PASSWORD=\"YourPassword\" when running cmake, or set it in CMakeLists.txt.")
endif()

# Define the IP address to ping
if(NOT DEFINED PING_TARGET_IP_STR)
    message(FATAL_ERROR "IP Adress fpr ping app is missing: PING_TARGET_IP_STR is not defined. Please define it via -DPING_IP_ADDRESSPING_TARGET_IP_STR=\"YourIPAdresstoPing\" when running cmake, or set it in CMakeLists.txt.")
endif()

# Define the MQTT server
if(NOT DEFINED MQTT_SERVER)
    message(FATAL_ERROR "MQTT Broker IP Adress is missing: MQTT_SERVER is not defined. Please define it via -DMQTT_SERVER=\"YourMQTTBrokerIPAdress\" when running cmake, or set it in CMakeLists.txt.")
endif()

# Define the MQTT port (optional, defaults to 1883)
if(NOT DEFINED MQTT_PORT)
    set(MQTT_PORT "1883")
endif()

add_compile_definitions(
    ${PROJECT_NAME}
    WIFI_SSID="${WIFI_SSID}"
    WIFI_PASSWORD="${WIFI_PASSWORD}"
    PING_TARGET_IP_STR="${PING_TARGET_IP_STR}"
    MQTT_SERVER="${MQTT_SERVER}"
    MQTT_PORT=${MQTT_PORT}
    $<$<BOOL:${MQTT_USERNAME}>:MQTT_USERNAME="${MQTT_USERNAME}">
    $<$<BOOL:${MQTT_PASSWORD}>:MQTT_PASSWORD="${MQTT_PASSWORD}">
)

