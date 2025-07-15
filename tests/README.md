# Test Suite for Pico Home Assistant

This is the test system for the Pico Home Assistant project. Tests run on the Linux host machine rather than on Pico hardware, enabling fast development and debugging.

## 🏗️ **Architecture**

```
tests/
├── CMakeLists.txt          # Test-specific CMake configuration
├── unit/                   # Unit tests (isolated components)
│   ├── test_ds18b20.c     # DS18B20 utility functions
│   ├── test_utils.c       # General utility functions
│   ├── ds18b20_utils.c    # Testable DS18B20 functions
│   └── mocks/              # Mock implementations for Pico SDK
│       ├── mock_gpio.c    # GPIO mock for Linux
│       └── mock_pico.c    # Pico SDK core functions mock
├── integration/            # Integration tests
│   └── test_full_system.c # End-to-end system tests
├── fixtures/               # Test helper functions
│   ├── test_helpers.c     # Common test utilities
│   └── test_helpers.h
└── vendor/                 # External test frameworks
    └── Unity/              # Unity test framework
```

## 🚀 **Quick Start**

### Run all tests
```bash
# From project root directory
./run_tests.sh
```

### VS Code Integration

For an optimal developer experience, tests can be run directly in VS Code:

**Installation:**
1. Install recommended extensions (VS Code suggests automatically)
2. Open Test Explorer (`Ctrl+Shift+P` → `Test: Show Output`)

**Usage:**
- **Run tests:** Click ▶️ in Test Explorer
- **Debug tests:** Click 🐛 for debugging with breakpoints  
- **Tasks:** `Ctrl+Shift+P` → `Tasks: Run Task` → select test category

See [VS Code Testing Guide](../docs/VS_CODE_TESTING.md) for detailed instructions.

### Manual execution
```bash
# Build tests
cd tests
cmake -B build -S .
cmake --build build

# Run tests
cd build
ctest --output-on-failure
```

### Build from main project (optional)
```bash
# Enable tests in main build
cmake -B build -S . -DBUILD_TESTS=ON

# Run tests via main project
make -C build run_tests
```

## 🧪 **Test Framework**

**Unity**: Lightweight C test framework popular for embedded development
- Header-only for simple tests
- No external dependencies
- Excellent error reporting
- Support for custom assertions

## 🎭 **Mock System**

### GPIO Mock (`mock_gpio.c`)
- Simulates Pico SDK GPIO functions
- Tracks state for all GPIO pins (0-29)
- Validates calls (init, set_dir, put, get)
- Call count tracking for verification

### Pico Core Mock (`mock_pico.c`)
- Simulates timing functions (sleep_ms, sleep_us)
- Mock time tracking with auto-increment
- stdio_init_all() and other core functions

## 📊 **Test Categories**

### Unit Tests
- **test_ds18b20.c**: Tests DS18B20 utility functions
  - Celsius to Fahrenheit conversion
  - Error string mapping
  - Temperature range validation
  - Mock GPIO behavior

- **test_utils.c**: Tests general utilities
  - String formatting
  - Version handling
  - Time mock functionality
  - Helper function reliability

### Integration Tests
- **test_full_system.c**: End-to-end system tests
  - System initialization sequence
  - Sensor communication protocol simulation
  - Error handling scenarios
  - Timing requirements validation
  - Resource cleanup verification

## 🔧 **Developing New Tests**

### Add a unit test
1. Create test file: `tests/unit/test_mynewfeature.c`
2. Add to CMakeLists.txt: `add_unit_test(test_mynewfeature unit/test_mynewfeature.c)`
3. Implement with Unity macros:
   ```c
   #include "unity.h"
   #include "test_helpers.h"
   
   void test_my_function(void) {
       TEST_ASSERT_EQUAL(42, my_function());
   }
   
   int main(void) {
       UNITY_BEGIN();
       RUN_TEST(test_my_function);
       return UNITY_END();
   }
   ```

### Add mock functionality
1. Implement mock in `tests/unit/mocks/`
2. Add mock control functions
3. Update `pico_mocks` library in CMakeLists.txt

## 📈 **Code Coverage**

To generate coverage report (requires lcov):
```bash
# Install tools
sudo apt install lcov

# Run tests with coverage
./run_tests.sh

# Open report
firefox tests/build/coverage_html/index.html
```

## 🎯 **Best Practices**

### Test Organization
- One test per function/feature
- Clear test names describing what is tested
- Group related tests in same file
- Use `setUp()` and `tearDown()` for cleanup

### Mock Usage
- Always reset mocks between tests
- Use state inspection for verification
- Simulate both success and error scenarios
- Test edge cases and error conditions

### Assertions
- Use specific Unity assertions (TEST_ASSERT_EQUAL, TEST_ASSERT_FLOAT_WITHIN)
- Include descriptive error messages
- Test both positive and negative cases
- Validate all output, not just return values

## 🔍 **Debugging**

### Verbose test output
```bash
cd tests/build
ctest --verbose
```

### Debug specific test
```bash
cd tests/build
gdb ./test_ds18b20
(gdb) run
```

### Print debugging
All mock functions write debug output showing what happens.

## 🌟 **Benefits of this approach**

1. **Fast tests**: Run on Linux instead of embedded target
2. **Deterministic**: Mocks provide controllable environment
3. **CI/CD friendly**: Easy to integrate in automated builds
4. **Debug-friendly**: Can use standard Linux debug tools
5. **Isolated**: Can test components independent of hardware
6. **Scalable**: Easy to add new tests as project grows
