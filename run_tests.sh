#!/bin/bash

# Test runner script for pico-home-assistant
# Builds and runs tests on Linux host platform

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}  Pico Home Assistant Test Suite${NC}"
echo -e "${BLUE}======================================${NC}"

# Check if we're in the right directory
if [[ ! -f "CMakeLists.txt" ]]; then
    echo -e "${RED}Error: Please run this script from the project root directory${NC}"
    exit 1
fi

# Create test build directory
TEST_BUILD_DIR="tests/build"
mkdir -p "$TEST_BUILD_DIR"

echo -e "${YELLOW}Building tests...${NC}"

# Configure CMake for tests
cd tests
cmake -B build -S .

# Build all tests
cmake --build build

echo -e "${GREEN}✓ Tests built successfully${NC}"

echo -e "${YELLOW}Running tests...${NC}"

# Run all tests
cd build
ctest --output-on-failure

TEST_RESULT=$?

if [ $TEST_RESULT -eq 0 ]; then
    echo -e "${GREEN}✓ All tests passed!${NC}"
else
    echo -e "${RED}✗ Some tests failed${NC}"
    exit $TEST_RESULT
fi

# Generate coverage report if lcov is available
if command -v lcov &> /dev/null; then
    echo -e "${YELLOW}Generating coverage report...${NC}"
    
    # Create coverage info
    lcov --capture --directory . --output-file coverage.info
    lcov --remove coverage.info '/usr/*' --output-file coverage.info
    lcov --remove coverage.info '*/vendor/*' --output-file coverage.info
    lcov --remove coverage.info '*/tests/*' --output-file coverage.info
    
    if command -v genhtml &> /dev/null; then
        genhtml coverage.info --output-directory coverage_html
        echo -e "${GREEN}✓ Coverage report generated in tests/build/coverage_html/${NC}"
    else
        echo -e "${YELLOW}⚠ genhtml not found, HTML coverage report not generated${NC}"
    fi
else
    echo -e "${YELLOW}⚠ lcov not found, coverage report not generated${NC}"
fi

echo -e "${BLUE}======================================${NC}"
echo -e "${GREEN}Test run completed successfully!${NC}"
echo -e "${BLUE}======================================${NC}"
