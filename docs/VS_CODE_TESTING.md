# VS Code Test Integration Guide

Detta dokument beskriver hur du kör och debuggar tester direkt i VS Code med Test Explorer.

## 🔧 **Installation**

### Rekommenderade Extensions

VS Code kommer att föreslå dessa extensions automatiskt när du öppnar projektet:

1. **C/C++** (`ms-vscode.cpptools`) - C/C++ support
2. **C/C++ Extension Pack** (`ms-vscode.cpptools-extension-pack`) - Komplett C++ toolkit
3. **CMake Tools** (`ms-vscode.cmake-tools`) - CMake integration
4. **C++ TestMate** (`matepek.vscode-catch2-test-adapter`) - Test adapter för C++
5. **Test Explorer UI** (`hbenl.vscode-test-explorer`) - Test Explorer UI
6. **Test Adapter Converter** (`ms-vscode.test-adapter-converter`) - Converter för test adapters

### Installation via Command Palette

```
Ctrl+Shift+P → Extensions: Show Recommended Extensions
```

Installera alla rekommenderade extensions.

## 🧪 **Köra Tester**

### Via Test Explorer

1. **Öppna Test Explorer:**
   - `Ctrl+Shift+P` → `Test: Show Output`
   - Eller klicka på Test-ikonen i Activity Bar

2. **Bygg och kör tester:**
   - Test Explorer hittar automatiskt dina tester
   - Klicka på ▶️ bredvid test-namnet för att köra
   - Klicka på 🔄 för att uppdatera test-listan

### Via Command Palette

```
Ctrl+Shift+P → Test: Run All Tests
Ctrl+Shift+P → Test: Run Tests in Current File
```

### Via Tasks

1. **Alla tester:**
   ```
   Ctrl+Shift+P → Tasks: Run Task → Run All Tests
   ```

2. **Specifika test-grupper:**
   ```
   Ctrl+Shift+P → Tasks: Run Task → Run DS18B20 Tests
   Ctrl+Shift+P → Tasks: Run Task → Run Utility Tests  
   Ctrl+Shift+P → Tasks: Run Task → Run Integration Tests
   ```

3. **Bygga tester:**
   ```
   Ctrl+Shift+P → Tasks: Run Task → Build Test Suite Only
   ```

## 🐛 **Debugging Tester**

### Via Test Explorer

1. Klicka på 🐛 bredvid test-namnet
2. Sätt breakpoints i test-koden
3. Debuggern startar automatiskt

### Via Debug Panel

1. **Öppna Debug Panel:** `Ctrl+Shift+D`
2. **Välj debug configuration:**
   - "Debug DS18B20 Tests"
   - "Debug Utility Tests"
   - "Debug Integration Tests"
3. **Starta debugging:** `F5`

### Debug Features

- **Breakpoints:** Klicka i vänstermarginalen
- **Step through:** `F10` (step over), `F11` (step into)
- **Variables:** Se värden i VARIABLES panel
- **Watch:** Lägg till variabler i WATCH panel
- **Call Stack:** Se anropsstack i CALL STACK panel

## 📊 **Test Output**

### Test Results

Test Explorer visar:
- ✅ **Passed tests** - Gröna checkmarks
- ❌ **Failed tests** - Röda kryss
- ⏸️ **Skipped tests** - Gul paus-ikon
- 🏃 **Running tests** - Spinner

### Detailed Output

1. **Test Output Panel:**
   ```
   View → Output → Select "Test Results"
   ```

2. **Terminal Output:**
   ```
   View → Terminal
   ```

3. **Klicka på test-namn** för detaljerad output

## ⚙️ **Konfiguration**

### Test Discovery

Test Explorer hittar automatiskt:
- `tests/build/test_ds18b20`
- `tests/build/test_utils`
- `tests/build/test_integration`

### Custom Settings

Redigera `.vscode/settings.json` för att anpassa:

```json
{
    "testMate.cpp.test.runtimeLimit": 60,
    "testMate.cpp.discovery.runtimeLimit": 30,
    "cmake.ctest.parallelJobs": 4
}
```

## 🚨 **Troubleshooting**

### Tester visas inte i Test Explorer

1. **Bygg testerna först:**
   ```
   Ctrl+Shift+P → Tasks: Run Task → Build Test Suite Only
   ```

2. **Refresh Test Explorer:**
   ```
   Ctrl+Shift+P → Test: Refresh Tests
   ```

3. **Kontrollera att executables finns:**
   ```bash
   ls tests/build/test_*
   ```

### Test Explorer Extension inte installerad

1. **Installera C++ TestMate:**
   ```
   Ctrl+Shift+P → Extensions: Install Extensions
   Sök: "C++ TestMate"
   ```

2. **Restart VS Code** efter installation

### CMake konfigurering

Om CMake inte hittar tester:

1. **Konfigurera test-build:**
   ```
   Ctrl+Shift+P → Tasks: Run Task → Configure Tests
   ```

2. **Kontrollera CMake output:**
   ```
   View → Output → Select "CMake/Build"
   ```

## 💡 **Tips & Tricks**

### Keyboard Shortcuts

- `Ctrl+;` → Fokusera Test Explorer
- `F5` → Debug current test
- `Ctrl+F5` → Run current test utan debugging
- `Shift+F5` → Stoppa debugging

### Test Organization

Test Explorer grupperar tester enligt:
```
📁 DS18B20 Tests
  └── test_celsius_to_fahrenheit_conversion
  └── test_error_strings
  └── ...
📁 Utility Tests  
  └── test_string_utilities
  └── test_mock_time_functions
  └── ...
📁 Integration Tests
  └── test_system_initialization
  └── test_sensor_reading_simulation
  └── ...
```

### Quick Actions

- **Right-click på test** → Context menu med run/debug options
- **Hover över test-namn** → Se senaste resultat
- **Click på line number** i test output → Hoppa till kod

### Auto-run Tests

För att köra tester automatiskt när kod ändras:

```json
{
    "testMate.cpp.discovery.loadOnStartup": true,
    "testMate.cpp.test.randomGeneratorSeed": null
}
```

## 🎯 **Workflow Rekommendationer**

### Utveckling av nya features

1. **Skriv test först** (TDD approach)
2. **Kör test** → ska misslyckas
3. **Implementera feature**
4. **Kör test igen** → ska lyckas
5. **Refactor kod**
6. **Kör alla tester** → ska lyckas

### Debugging workflow

1. **Identifiera failing test** i Test Explorer
2. **Sätt breakpoint** i test-kod
3. **Debug test** med `F5`
4. **Step through kod** för att förstå problemet
5. **Fixa kod**
6. **Kör test igen**

### Coverage workflow

1. **Kör alla tester** via terminal
2. **Generera coverage** med `./run_tests.sh`
3. **Öppna coverage report** i browser
4. **Identifiera untested kod**
5. **Skriv fler tester**
