# QP/C++ ESP-IDF Component Initialization
# This file is included when qpcpp is being built as an ESP-IDF component

message(STATUS "QP/C++ ESP-IDF Component: ${COMPONENT_NAME}")

if(CMAKE_BUILD_EARLY_EXPANSION)
    idf_component_register(
        REQUIRES freertos
    )
    return()
endif()

# Check if QPCPP_DIR is set
if(NOT DEFINED QPCPP_DIR OR QPCPP_DIR STREQUAL "" OR NOT EXISTS "${QPCPP_DIR}/include")
    message(FATAL_ERROR "QPCPP_DIR is invalid ('${QPCPP_DIR}'). Verify FetchContent in top-level CMakeLists.txt.")
endif()

# Set default port for ESP-IDF if not already set
if(NOT DEFINED QPCPP_PORT)
    set(QPCPP_PORT "esp-idf" CACHE STRING "QP/C++ port")
endif()

message(STATUS "QP/C++ Port: ${QPCPP_PORT}")

# Register the ESP-IDF component
idf_component_register(
    SRCS
        # qpcpp core sources
        "${QPCPP_DIR}/src/qf/qep_hsm.cpp"
        "${QPCPP_DIR}/src/qf/qep_msm.cpp"
        "${QPCPP_DIR}/src/qf/qf_act.cpp"
        "${QPCPP_DIR}/src/qf/qf_actq.cpp"
        "${QPCPP_DIR}/src/qf/qf_defer.cpp"
        "${QPCPP_DIR}/src/qf/qf_dyn.cpp"
        "${QPCPP_DIR}/src/qf/qf_mem.cpp"
        "${QPCPP_DIR}/src/qf/qf_ps.cpp"
        "${QPCPP_DIR}/src/qf/qf_qact.cpp"
        "${QPCPP_DIR}/src/qf/qf_qeq.cpp"
        "${QPCPP_DIR}/src/qf/qf_qmact.cpp"
        "${QPCPP_DIR}/src/qf/qf_time.cpp"
        "${QPCPP_DIR}/ports/esp-idf/qf_port.cpp"
    INCLUDE_DIRS
        # qpcpp headers
        "${QPCPP_DIR}/include"
        "${QPCPP_DIR}/src"
        "${QPCPP_DIR}/ports/esp-idf"
    REQUIRES freertos
)

# Create public qpcpp target for other components
add_library(qpcpp INTERFACE)
target_link_libraries(qpcpp INTERFACE ${COMPONENT_LIB})
target_include_directories(qpcpp INTERFACE
    ${QPCPP_DIR}/include
    ${QPCPP_DIR}/src
    ${QPCPP_DIR}/ports/esp-idf
)
