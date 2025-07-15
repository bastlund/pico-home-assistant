# Test Suite för Pico Home Assistant

Detta är testsystemet för Pico Home Assistant-projektet. Testerna körs på Linux-värddatorn snarare än på Pico-hårdvaran, vilket möjliggör snabb utveckling och debugging.

## 🏗️ **Arkitektur**

```
tests/
├── CMakeLists.txt          # Test-specifik CMake konfiguration
├── unit/                   # Unit tests (isolerade komponenter)
│   ├── test_ds18b20.c     # DS18B20 utility-funktioner
│   ├── test_utils.c       # Allmänna utility-funktioner
│   ├── ds18b20_utils.c    # Testbara DS18B20 funktioner
│   └── mocks/              # Mock-implementationer för Pico SDK
│       ├── mock_gpio.c    # GPIO mock för Linux
│       └── mock_pico.c    # Pico SDK core funktioner mock
├── integration/            # Integration tests
│   └── test_full_system.c # End-to-end system-tester
├── fixtures/               # Test hjälpfunktioner
│   ├── test_helpers.c     # Gemensamma test utilities
│   └── test_helpers.h
└── vendor/                 # Externa test frameworks
    └── Unity/              # Unity test framework
```

## 🚀 **Snabbstart**

### Kör alla tester
```bash
# Från projektets rotmapp
./run_tests.sh
```

### VS Code Integration

För en optimal utvecklarupplevelse kan testerna köras direkt i VS Code:

**Installation:**
1. Installera rekommenderade extensions (VS Code föreslår automatiskt)
2. Öppna Test Explorer (`Ctrl+Shift+P` → `Test: Show Output`)

**Användning:**
- **Kör tester:** Klicka ▶️ i Test Explorer
- **Debug tester:** Klicka 🐛 för debugging med breakpoints  
- **Tasks:** `Ctrl+Shift+P` → `Tasks: Run Task` → välj test-kategori

Se [VS Code Testing Guide](../docs/VS_CODE_TESTING.md) för detaljerade instruktioner.

### Manuell körning
```bash
# Bygg testerna
cd tests
cmake -B build -S .
cmake --build build

# Kör testerna
cd build
ctest --output-on-failure
```

### Bygg från huvudprojektet (valfritt)
```bash
# Aktivera tester i huvudbygget
cmake -B build -S . -DBUILD_TESTS=ON

# Kör tester via huvudprojektet
make -C build run_tests
```

## 🧪 **Test Framework**

**Unity**: Lightweight C test framework som är populärt för embedded development
- Header-only för enkla tester
- Inga externa beroenden
- Utmärkt felrapportering
- Stöd för custom assertions

## 🎭 **Mock System**

### GPIO Mock (`mock_gpio.c`)
- Simulerar Pico SDK GPIO-funktioner
- Spårar tillstånd för alla GPIO-pins (0-29)
- Validerar anrop (init, set_dir, put, get)
- Call count tracking för verifiering

### Pico Core Mock (`mock_pico.c`)
- Simulerar timing-funktioner (sleep_ms, sleep_us)
- Mock time tracking med auto-increment
- stdio_init_all() och andra core funktioner

## 📊 **Testkategorier**

### Unit Tests
- **test_ds18b20.c**: Testar DS18B20 utility-funktioner
  - Celsius till Fahrenheit konvertering
  - Error string mappning
  - Temperature range validation
  - Mock GPIO beteende

- **test_utils.c**: Testar allmänna utilities
  - String formatting
  - Version hantering
  - Time mock funktionalitet
  - Helper function reliability

### Integration Tests
- **test_full_system.c**: End-to-end system tester
  - System initialization sekvens
  - Sensor communication protocol simulation
  - Error handling scenarios
  - Timing requirements validation
  - Resource cleanup verification

## 🔧 **Utveckling av Nya Tester**

### Lägg till ett unit test
1. Skapa test-fil: `tests/unit/test_mynewfeature.c`
2. Lägg till i CMakeLists.txt: `add_unit_test(test_mynewfeature unit/test_mynewfeature.c)`
3. Implementera med Unity macros:
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

### Lägg till mock-funktionalitet
1. Implementera mock i `tests/unit/mocks/`
2. Lägg till mock-kontroll funktioner
3. Uppdatera `pico_mocks` library i CMakeLists.txt

## 📈 **Code Coverage**

För att generera coverage-rapport (kräver lcov):
```bash
# Installera verktyg
sudo apt install lcov

# Kör tester med coverage
./run_tests.sh

# Öppna rapport
firefox tests/build/coverage_html/index.html
```

## 🎯 **Best Practices**

### Test Organization
- Ett test per funktion/feature
- Klara test-namn som beskriver vad som testas
- Gruppera relaterade tester i samma fil
- Använd `setUp()` och `tearDown()` för cleanup

### Mock Usage
- Återställ alltid mocks mellan tester
- Använd state inspection för verifiering
- Simulera både success och error scenarios
- Testa edge cases och error conditions

### Assertions
- Använd specifika Unity assertions (TEST_ASSERT_EQUAL, TEST_ASSERT_FLOAT_WITHIN)
- Inkludera beskrivande felmeddelanden
- Testa både positiva och negativa fall
- Validera all output, inte bara return values

## 🔍 **Debugging**

### Verbose test output
```bash
cd tests/build
ctest --verbose
```

### Debug specifikt test
```bash
cd tests/build
gdb ./test_ds18b20
(gdb) run
```

### Print debugging
Alla mock-funktioner skriver debug-output som visar vad som händer.

## 🌟 **Fördelar med denna approach**

1. **Snabba tester**: Körs på Linux istället för embedded target
2. **Deterministiska**: Mocks ger kontrollerbar miljö
3. **CI/CD friendly**: Lätt att integrera i automated builds
4. **Debug-vänlig**: Kan använda standard Linux debug-verktyg
5. **Isolerad**: Kan testa komponenter oberoende av hårdvara
6. **Skalbar**: Lätt att lägga till nya tester när projektet växer
