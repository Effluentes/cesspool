# QP/C++ Quick Reference - After Refactoring

## 🎯 For Application Developers Using QP/C++

### In CMakeLists.txt

**For ESP32 applications:**
```cmake
idf_component_register(
    SRCS src/main.cpp
    REQUIRES qpcpp       # This now includes ESP-IDF port automatically
)
```

**For portable applications (POSIX/Linux):**
```cmake
# Ensure qpcpp is fetched
include(FetchContent)
FetchContent_Declare(qpcpp
    GIT_REPOSITORY https://github.com/vChavezB/qpcpp.git
    GIT_TAG origin/esp32
)
FetchContent_Populate(qpcpp)
set(QPCPP_DIR ${qpcpp_SOURCE_DIR})

# Set port based on platform
if(WIN32 OR UNIX)
    set(QPCPP_PORT "posix")
else()
    set(QPCPP_PORT "generic")
endif()

# Add qpcpp library
add_subdirectory(packages/third_party/qpcpp)

# Link to your application
add_executable(myapp src/main.cpp)
target_link_libraries(myapp qpcpp)
```

### In C++ Code

**Include headers:**
```cpp
#include <qpcpp.hpp>              // Core QP/C++ headers

// For ESP32 specific utilities:
#include <qpcpp/EspIdfPort.hh>    // Old path (legacy compatibility)
// OR
#include <qpcpp_esp32_port/EspIdfPort.hh>  // New path
```

**Using QP/C++ types:**
```cpp
namespace QP {
    class MyAO : public QP::QActive {
    public:
        MyAO() : QP::QActive(Q_STATE_CAST(&MyAO::initial)) {}

    protected:
        Q_STATE_DECL(initial);
        Q_STATE_DECL(active);
    };
}
```

## 🔧 For Developers Maintaining Components

### Creating a New Active Object Component

```cmake
# CMakeLists.txt
idf_component_register(
    SRCS
        src/MyActiveObject.cpp
    INCLUDE_DIRS
        include
    REQUIRES
        qpcpp
        qpcpp_esp32_port      # Only if using ESP32-specific types
        freertos
        driver                # If needed
)
```

```cpp
// include/my_ao/MyActiveObject.hh
#pragma once
#include <qpcpp.hpp>
#include <qpcpp/EspIdfPort.hh>   // For ESP32 type aliases

using EventQueue = QP::QEvt const **;
using Priority = std::uint_fast8_t;

namespace MyNamespace {
    class MyActiveObject : public QP::QActive {
        // ...
    };
}
```

## ⚙️ Platform-Specific Code

### For ESP32 Specific Code

**File:** `platform/esp32/components/my_component/CMakeLists.txt`
```cmake
idf_component_register(
    SRCS src/MyEsp32Driver.cpp
    INCLUDE_DIRS include
    REQUIRES qpcpp freertos driver
)
```

**File:** `platform/esp32/components/my_component/src/MyEsp32Driver.cpp`
```cpp
#include "my_component/MyEsp32Driver.hh"
#include <esp_log.h>

void MyEsp32Driver::init() {
    ESP_LOGI("MyDriver", "Initializing...");
    // ESP32-specific code
}
```

### For Platform-Independent Code

**File:** `packages/domain/my_feature/CMakeLists.txt`
```cmake
# Platform-independent CMakeLists.txt
add_library(my_feature STATIC)
target_sources(my_feature PRIVATE src/MyFeature.cpp)
target_include_directories(my_feature PUBLIC include)
target_link_libraries(my_feature PUBLIC qpcpp)  # Pure library reference
```

**File:** `packages/domain/my_feature/include/my_feature/MyFeature.hh`
```cpp
#pragma once
#include <qpcpp.hpp>

class MyFeature {
    // Platform-independent code
};
```

## 🔄 Migration from Old Structure

### Old Code Pattern
```cpp
// Component required
REQUIRES qpcpp

// Include pattern
#include <qpcpp.hpp>
#include <qpcpp/EspIdfPort.hh>
```

### No Changes Needed! ✅

The old code continues to work exactly as before. The refactoring is fully backward compatible.

### Optional: Adopt New Structure

For new code, you can use:
```cpp
// More explicit ESP32 dependencies
REQUIRES qpcpp qpcpp_esp32_port

// New include path (optional)
#include <qpcpp_esp32_port/EspIdfPort.hh>
```

## 📍 Component Locations & Purposes

| Component | Location | Purpose | Depends On |
|-----------|----------|---------|-----------|
| `qpcpp` | `packages/third_party/qpcpp/` | Core QP/C++ library | FreeRTOS (IDF) |
| `qpcpp_esp32_port` | `platform/esp32/qpcpp_esp32_port/` | ESP32 abstractions | qpcpp, FreeRTOS |
| (legacy) `qpcpp` | `platform/esp32/components/qpcpp/` | Backward compat wrapper | qpcpp, qpcpp_esp32_port |

## 🐛 Troubleshooting

### Error: "qpcpp target not found"
**Cause:** qpcpp component not in EXTRA_COMPONENT_DIRS
**Solution:** Check that `packages/third_party` is in EXTRA_COMPONENT_DIRS in root CMakeLists.txt

### Error: "QPCPP_DIR is invalid"
**Cause:** FetchContent didn't populate qpcpp
**Solution:** Ensure FetchContent_Declare and FetchContent_Populate for qpcpp happens before setting EXTRA_COMPONENT_DIRS

### Old includes not working
**Cause:** Using new include paths incorrectly
**Solution:** Include paths are identical, just verify component dependencies are correct

## 📚 See Also

- [QPCPP_REFACTORING.md](./QPCPP_REFACTORING.md) - Full architecture documentation
- [QPCPP_REFACTORING_CHANGES.md](./QPCPP_REFACTORING_CHANGES.md) - File changes summary
- [QP/C++ Official Documentation](https://www.state-machine.com/qpc)
