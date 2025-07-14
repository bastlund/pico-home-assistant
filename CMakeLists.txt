cmake_minimum_required(VERSION 3.13)

# Enable ccache for faster builds
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
endif()

include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

# Project version
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 3)

# Try to get version from git describe
find_package(Git QUIET)
if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --always --dirty
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    
    if(GIT_VERSION)
        # If we have a git version, use it
        set(PROJECT_VERSION_FULL "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-${GIT_VERSION}")
    else()
        set(PROJECT_VERSION_FULL "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
    endif()
else()
    set(PROJECT_VERSION_FULL "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
endif()

set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

project(pico_w VERSION ${PROJECT_VERSION} LANGUAGES C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# Add version information as compile definitions
add_compile_definitions(
    PROJECT_VERSION="${PROJECT_VERSION}"
    PROJECT_VERSION_FULL="${PROJECT_VERSION_FULL}"
    PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    PROJECT_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

# Scan application - uses scan.c, not ping.c
add_executable(pico_w_scan src/main/scan.c src/utils/version_display.c)
target_include_directories(pico_w_scan PRIVATE 
    ${CMAKE_CURRENT_LIST_DIR}/src/utils
    ${CMAKE_CURRENT_LIST_DIR}/src/config
)
target_link_libraries(pico_w_scan pico_stdlib pico_cyw43_arch_lwip_threadsafe_background)
pico_add_extra_outputs(pico_w_scan)
pico_enable_stdio_usb(pico_w_scan 1)
pico_enable_stdio_uart(pico_w_scan 0)

# Ping application
add_executable(pico_w_ping src/main/ping.c src/utils/version_display.c)
target_include_directories(pico_w_ping PRIVATE 
    ${CMAKE_CURRENT_LIST_DIR}/src/utils
    ${CMAKE_CURRENT_LIST_DIR}/src/config
)
target_link_libraries(pico_w_ping pico_stdlib pico_cyw43_arch_lwip_threadsafe_background)
pico_add_extra_outputs(pico_w_ping)
pico_enable_stdio_usb(pico_w_ping 1)
pico_enable_stdio_uart(pico_w_ping 0)

# Note: pico-onewire library removed due to C++ incompatibility with C project

# DS18B20 static library
add_library(ds18b20_lib STATIC src/drivers/ds18b20.c)
target_include_directories(ds18b20_lib PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src/drivers)
target_link_libraries(ds18b20_lib
    pico_stdlib
    hardware_gpio
    hardware_sync
    pico_time
)

# Sensor application
add_executable(pico_w_sensor src/main/sensor.c src/utils/version_display.c)
target_include_directories(pico_w_sensor PRIVATE 
    ${CMAKE_CURRENT_LIST_DIR}/src/utils 
    ${CMAKE_CURRENT_LIST_DIR}/src/drivers
    ${CMAKE_CURRENT_LIST_DIR}/src/config
)
target_link_libraries(pico_w_sensor
    pico_stdlib
    hardware_adc
    ds18b20_lib
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_mqtt
    pico_mbedtls
    pico_lwip_mbedtls
)
pico_add_extra_outputs(pico_w_sensor)
pico_enable_stdio_usb(pico_w_sensor 1)
pico_enable_stdio_uart(pico_w_sensor 0)

# DS18B20 test application - using pure C implementation
add_executable(pico_w_ds18b20_test tests/ds18b20_test.c src/utils/version_display.c)
target_include_directories(pico_w_ds18b20_test PRIVATE 
    ${CMAKE_CURRENT_LIST_DIR}/src/utils 
    ${CMAKE_CURRENT_LIST_DIR}/src/drivers
    ${CMAKE_CURRENT_LIST_DIR}/src/config
)
target_link_libraries(pico_w_ds18b20_test
    pico_stdlib
    pico_cyw43_arch_lwip_threadsafe_background
    ds18b20_lib
)
pico_add_extra_outputs(pico_w_ds18b20_test)
pico_enable_stdio_usb(pico_w_ds18b20_test 1)
pico_enable_stdio_uart(pico_w_ds18b20_test 0)

# Add compile definitions for DS18B20 test
target_compile_definitions(pico_w_ds18b20_test PRIVATE
    PROJECT_VERSION_FULL="${PROJECT_VERSION_FULL}"
    PROJECT_VERSION="${PROJECT_VERSION}"
    PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    PROJECT_VERSION_PATCH=${PROJECT_VERSION_PATCH}
    DEBUG_LEVEL=${DEBUG_LEVEL}
    DS18B20_GPIO_PIN=${DS18B20_GPIO_PIN}
)

# Define the WiFi credentials
if(NOT DEFINED WIFI_SSID)
    message(FATAL_ERROR "WIFI credentials missing: WIFI_SSID is not defined. Please define it via -DWIFI_SSID=\"YourSSID\" when running cmake, or set it in CMakeLists.txt.")
endif()

if(NOT DEFINED WIFI_PASSWORD)
    message(FATAL_ERROR "WIFI credentials missing: WIFI_PASSWORD is not defined. Please define it via -DWIFI_PASSWORD=\"YourPassword\" when running cmake, or set it in CMakeLists.txt.")
endif()

# Define the IP address to ping
if(NOT DEFINED PING_TARGET_IP_STR)
    message(FATAL_ERROR "IP address for ping app is missing: PING_TARGET_IP_STR is not defined. Please define it via -DPING_TARGET_IP_STR=\"YourIPAddressToPing\" when running cmake, or set it in CMakeLists.txt.")
endif()

# Define the MQTT server
if(NOT DEFINED MQTT_SERVER)
    message(FATAL_ERROR "MQTT Broker IP address is missing: MQTT_SERVER is not defined. Please define it via -DMQTT_SERVER=\"YourMQTTBrokerIPAddress\" when running cmake, or set it in CMakeLists.txt.")
endif()

# Define the MQTT port (optional, defaults to 1883)
if(NOT DEFINED MQTT_PORT)
    set(MQTT_PORT "1883")
endif()

# Define debug level (optional, defaults to 2)
if(NOT DEFINED DEBUG_LEVEL)
    set(DEBUG_LEVEL "2")
endif()

# Define DS18B20 GPIO pin (optional, defaults to 2)
if(NOT DEFINED DS18B20_GPIO_PIN)
    set(DS18B20_GPIO_PIN "2")
endif()

# Define the debug level (optional, defaults to 2)
if(NOT DEFINED DEBUG_LEVEL)
    set(DEBUG_LEVEL "2")
endif()

add_compile_definitions(
    ${PROJECT_NAME}
    WIFI_SSID="${WIFI_SSID}"
    WIFI_PASSWORD="${WIFI_PASSWORD}"
    PING_TARGET_IP_STR="${PING_TARGET_IP_STR}"
    MQTT_SERVER="${MQTT_SERVER}"
    MQTT_PORT=${MQTT_PORT}
    DEBUG_LEVEL=${DEBUG_LEVEL}
    $<$<BOOL:${MQTT_USERNAME}>:MQTT_USERNAME="${MQTT_USERNAME}">
    $<$<BOOL:${MQTT_PASSWORD}>:MQTT_PASSWORD="${MQTT_PASSWORD}">
)

