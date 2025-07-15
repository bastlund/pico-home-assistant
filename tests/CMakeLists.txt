# Test CMakeLists.txt - Builds tests for Linux host platform
cmake_minimum_required(VERSION 3.13)

project(pico_home_assistant_tests LANGUAGES C CXX)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Enable testing
enable_testing()

# Compiler flags for better debugging and warnings
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g -O0 --coverage")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g -O0 --coverage")

# Unity test framework
set(UNITY_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/vendor/Unity)
add_library(unity STATIC
    ${UNITY_ROOT}/src/unity.c
)
target_include_directories(unity PUBLIC
    ${UNITY_ROOT}/src
)

# Test fixtures library
add_library(test_fixtures STATIC
    fixtures/test_helpers.c
)
target_include_directories(test_fixtures PUBLIC
    fixtures
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)
target_link_libraries(test_fixtures unity)

# Mock library for Pico SDK functions
add_library(pico_mocks STATIC
    unit/mocks/mock_gpio.c
    unit/mocks/mock_pico.c
)
target_include_directories(pico_mocks PUBLIC
    unit/mocks
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)
target_link_libraries(pico_mocks unity)

# Include directories for source code under test
set(SRC_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/drivers
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/config
)

# Function to create a test executable
function(add_unit_test test_name test_file)
    # Handle additional source files
    set(extra_sources ${ARGN})
    
    add_executable(${test_name} ${test_file} ${extra_sources})
    target_include_directories(${test_name} PRIVATE
        ${SRC_INCLUDE_DIRS}
        unit/mocks
        unit
    )
    target_link_libraries(${test_name} 
        unity 
        test_fixtures 
        pico_mocks
        gcov
    )
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

# Add unit tests
add_unit_test(test_ds18b20 unit/test_ds18b20.c unit/ds18b20_utils.c)
add_unit_test(test_utils unit/test_utils.c)

# Integration tests
add_executable(test_integration integration/test_full_system.c)
target_include_directories(test_integration PRIVATE
    ${SRC_INCLUDE_DIRS}
    unit/mocks
)
target_link_libraries(test_integration 
    unity 
    test_fixtures 
    pico_mocks
    gcov
)
add_test(NAME test_integration COMMAND test_integration)

# Custom target to run all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_ds18b20 test_utils test_integration
    COMMENT "Running all tests"
)

# Custom target for coverage report
add_custom_target(coverage
    COMMAND lcov --capture --directory . --output-file coverage.info
    COMMAND lcov --remove coverage.info '/usr/*' --output-file coverage.info
    COMMAND lcov --remove coverage.info '*/vendor/*' --output-file coverage.info
    COMMAND genhtml coverage.info --output-directory coverage_html
    COMMENT "Generating code coverage report"
)
