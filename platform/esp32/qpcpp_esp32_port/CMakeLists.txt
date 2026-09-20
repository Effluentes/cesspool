message(STATUS "ESP component ${COMPONENT_NAME} - QP/C++ ESP32 Port Abstractions")

if(CMAKE_BUILD_EARLY_EXPANSION)
    idf_component_register(
        REQUIRES qpcpp freertos
    )
    return()
endif()

# Register as an ESP-IDF component
# This component provides ESP32-specific type aliases and abstractions for QP/C++
idf_component_register(
    SRCS
        "src/EspIdfPort.cc"
    INCLUDE_DIRS
        "include"
    REQUIRES qpcpp freertos
)

