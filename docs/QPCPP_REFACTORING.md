# QP/C++ Refactoring - New Architecture

## Overview
QP/C++ has been refactored to be more reusable and decoupled from ESP-IDF specifics. It's now available as a pure CMake library that can be used in any project, while still maintaining full ESP-IDF integration.

## New Folder Structure

### 1. **packages/third_party/qpcpp/** (Pure CMake Library)
- **Location**: `/workspaces/cesspool/packages/third_party/qpcpp/`
- **Role**: Main QP/C++ library
- **Features**:
  - Works as both a standalone CMake library AND an ESP-IDF component
  - Includes all QP/C++ core framework sources
  - Automatically includes ESP-IDF port when used as an IDF component
  - Can be used in non-ESP32 projects with different ports (posix, generic, etc.)

- **Usage in ESP-IDF projects**:
  ```cmake
  set(EXTRA_COMPONENT_DIRS
      ${CMAKE_SOURCE_DIR}/packages/third_party
      # ... other dirs
  )
  ```

- **Usage in standalone CMake projects**:
  ```cmake
  FetchContent_Declare(qpcpp ...)
  FetchContent_Populate(qpcpp)
  set(QPCPP_DIR ${qpcpp_SOURCE_DIR})
  set(QPCPP_PORT "posix")  # or "generic", "freertos", etc.
  add_subdirectory(packages/third_party/qpcpp)

  target_link_libraries(my_app qpcpp)
  ```

### 2. **platform/esp32/qpcpp_esp32_port/** (ESP32-Specific Port)
- **Location**: `/workspaces/cesspool/platform/esp32/qpcpp_esp32_port/`
- **Role**: ESP32-specific type aliases and abstractions
- **Contents**:
  - `src/EspIdfPort.cc` - QF startup/shutdown implementation
  - `include/qpcpp_esp32_port/EspIdfPort.hh` - Type aliases for active object configuration

- **Purpose**: Provides ES P32-specific utilities without duplicating library code

### 3. **platform/esp32/components/qpcpp/** (Legacy Wrapper - Deprecated)
- **Location**: `/workspaces/cesspool/platform/esp32/components/qpcpp/`
- **Role**: Backward compatibility wrapper
- **Status**: ⚠️ Deprecated - kept for compatibility
- **Behavior**: Simply forwards to qpcpp + qpcpp_esp32_port
- **Note**: New code should directly depend on `qpcpp` component instead

## Component Dependencies

### Before Refactoring (Tightly Coupled)
```
my_component
  └─ REQUIRES qpcpp (ESP-IDF component)
       └─ Contains all library sources (duplicated)
       └─ Contains ESP-IDF port
       └─ Depends on FreeRTOS
```

### After Refactoring (Modular)
```
my_component
  └─ REQUIRES qpcpp (pure library)
       └─ Core QP/C++ framework
       └─ ESP-IDF port (included automatically)
       └─ Depends on FreeRTOS
  └─ REQUIRES qpcpp_esp32_port (optional - for ESP32-specific types)
```

## Porting Guide

### For Existing Components

**Before:**
```cmake
idf_component_register(
    REQUIRES qpcpp freertos
)
```

**After:**
```cmake
# Option 1: Just qpcpp (for minimal dependency)
idf_component_register(
    REQUIRES qpcpp freertos
)

# Option 2: With ESP32 port abstractions
idf_component_register(
    REQUIRES qpcpp qpcpp_esp32_port freertos
)
```

### Include Paths

**Before:**
```cpp
#include <qpcpp.hpp>
#include <qpcpp/EspIdfPort.hh>
```

**After:**
```cpp
// Same as before (backward compatible)
#include <qpcpp.hpp>
#include <qpcpp/EspIdfPort.hh>

// Or use new paths:
#include <qpcpp.hpp>
#include <qpcpp_esp32_port/EspIdfPort.hh>
```

## Benefits

1. **Reusability**: QP/C++ can now be used in non-ESP32 projects
2. **Separation of Concerns**: ESP32-specific code is separate from the core library
3. **Easier Maintenance**: Single source of QP/C++ library
4. **Backward Compatibility**: Existing code continues to work
5. **Flexibility**: Can swap ESP-IDF port for other ports (POSIX, generic, etc.)

## File Changes Summary

| File | Change | Reason |
|------|--------|--------|
| `/CMakeLists.txt` | Updated to add qpcpp via EXTRA_COMPONENT_DIRS | Enable ESP-IDF component discovery |
| `packages/third_party/qpcpp/CMakeLists.txt` | Dual-mode: standalone + IDF component | Support both use cases |
| `platform/esp32/qpcpp_esp32_port/CMakeLists.txt` | Created new ESP32-specific port component | Separate platform-specific code |
| `platform/esp32/components/qpcpp/CMakeLists.txt` | Simplified to legacy wrapper | Maintain backward compatibility |

## Architecture Diagram

```
┌─────────────────────────────────────┐
│  Application (myapp)                │
└──────────┬──────────────────────────┘
           │ REQUIRES
           ├─────────────────────────────────────┐
           │                                     │
   ┌───────▼────────┐              ┌────────────▼──────┐
   │  qpcpp         │              │  qpcpp_esp32_port │
   │  (Pure Library)│              │  (ESP32 Specific) │
   └───────┬────────┘              └────────────┬──────┘
           │ REQUIRES                           │
           │                            ┌───────▼────────┐
           │                            │  qpcpp         │
           │                            │  (Pure Library)│
           │                            └────────────────┘
           │
    ┌──────▼──────────────┐
    │  FreeRTOS (IDF)     │
    └─────────────────────┘
```

## Future Enhancements

- Create similar port components for other platforms (POSIX, Linux, etc.)
- Add qpcpp package configuration file for better cross-project integration
- Consider publishing qpcpp wrapper as a reusable module in package managers
