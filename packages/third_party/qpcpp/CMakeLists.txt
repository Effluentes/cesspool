cmake_minimum_required(VERSION 3.22)

# QP/C++ Pure CMake Library & ESP-IDF Component
#
# This CMakeLists.txt provides two modes of operation:
# 1. As an ESP-IDF component (when COMPONENT_NAME is defined)
# 2. As a standalone CMake library (when used in non-ESP-IDF projects)
#
# When used as an ESP-IDF component, it builds the entire qpcpp library
# including the esp-idf port. This makes it fully self-contained and reusable.

# Check if we're being called as an ESP-IDF component
if(DEFINED COMPONENT_NAME)
    # ========== ESP-IDF COMPONENT MODE ==========
    message(STATUS "QP/C++ building as ESP-IDF component: ${COMPONENT_NAME}")

    if(CMAKE_BUILD_EARLY_EXPANSION)
        idf_component_register(
            REQUIRES freertos
        )
        return()
    endif()

    # Verify QPCPP_DIR is set
    if(NOT DEFINED QPCPP_DIR OR QPCPP_DIR STREQUAL "" OR NOT EXISTS "${QPCPP_DIR}/include")
        message(FATAL_ERROR "QPCPP_DIR is invalid ('${QPCPP_DIR}'). Verify FetchContent in top-level CMakeLists.txt.")
    endif()

    # Register the component with all qpcpp sources and the esp-idf port
    idf_component_register(
        SRCS
            # qpcpp core framework sources
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
            # esp-idf port implementation
            "${QPCPP_DIR}/ports/esp-idf/qf_port.cpp"
        INCLUDE_DIRS
            # Public headers for consumers
            "${QPCPP_DIR}/include"
            "${QPCPP_DIR}/src"
            "${QPCPP_DIR}/ports/esp-idf"
        REQUIRES freertos
    )

    add_compile_options(-Wno-error=volatile)
endif()

# ========== STANDALONE CMAKE MODE ==========
# For non-ESP-IDF usage

if(NOT PROJECT_NAME)
    project(qpcpp VERSION 7.3.0 LANGUAGES CXX C)
endif()

# Verify QPCPP_DIR is set
if(NOT DEFINED QPCPP_DIR OR QPCPP_DIR STREQUAL "" OR NOT EXISTS "${QPCPP_DIR}/include")
    message(FATAL_ERROR "QPCPP_DIR is not set or invalid. Ensure qpcpp is fetched via FetchContent before including this CMakeLists.txt")
endif()

message(STATUS "Configuring QP/C++ (standalone mode) from: ${QPCPP_DIR}")

# Set default port if not specified
if(NOT DEFINED QPCPP_PORT)
    set(QPCPP_PORT "generic" CACHE STRING "QP/C++ port: generic, freertos, posix, etc.")
endif()

message(STATUS "QP/C++ Port: ${QPCPP_PORT}")

# Create the qpcpp library
add_library(qpcpp STATIC)

# Add QP/C++ core sources
target_sources(qpcpp PRIVATE
    ${QPCPP_DIR}/src/qf/qep_hsm.cpp
    ${QPCPP_DIR}/src/qf/qep_msm.cpp
    ${QPCPP_DIR}/src/qf/qf_act.cpp
    ${QPCPP_DIR}/src/qf/qf_actq.cpp
    ${QPCPP_DIR}/src/qf/qf_defer.cpp
    ${QPCPP_DIR}/src/qf/qf_dyn.cpp
    ${QPCPP_DIR}/src/qf/qf_mem.cpp
    ${QPCPP_DIR}/src/qf/qf_ps.cpp
    ${QPCPP_DIR}/src/qf/qf_qact.cpp
    ${QPCPP_DIR}/src/qf/qf_qeq.cpp
    ${QPCPP_DIR}/src/qf/qf_qmact.cpp
    ${QPCPP_DIR}/src/qf/qf_time.cpp
)

# Add public headers
target_include_directories(qpcpp PUBLIC
    ${QPCPP_DIR}/include
    ${QPCPP_DIR}/src
)

# Add port-specific sources if they exist
if(EXISTS "${QPCPP_DIR}/ports/${QPCPP_PORT}")
    target_sources(qpcpp PRIVATE
        ${QPCPP_DIR}/ports/${QPCPP_PORT}/qf_port.cpp
    )
    target_include_directories(qpcpp PUBLIC
        ${QPCPP_DIR}/ports/${QPCPP_PORT}
    )
    message(STATUS "QP/C++ port files included from: ${QPCPP_DIR}/ports/${QPCPP_PORT}")
endif()

# Set C++ standard
set_target_properties(qpcpp PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    C_STANDARD 11
)

# Export for external use
include(GNUInstallDirs)

install(TARGETS qpcpp
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${QPCPP_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)


