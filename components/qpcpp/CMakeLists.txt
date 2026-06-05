message(STATUS "ESP component ${COMPONENT_NAME}")

if(CMAKE_BUILD_EARLY_EXPANSION)
    idf_component_register(
        REQUIRES freertos
    )
    return()
endif()

if(NOT DEFINED QPCPP_DIR OR QPCPP_DIR STREQUAL "" OR NOT EXISTS "${QPCPP_DIR}/include")
    message(FATAL_ERROR "QPCPP_DIR is invalid ('${QPCPP_DIR}'). Verify FetchContent in top-level CMakeLists.txt.")
endif()

idf_component_register(
    SRCS
        # missing q_assert implementation
        "src/EspIdfPort.cc"
        # qpcpp sources
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
        # missing q_assert implementation
        "include"
        # qpcpp headers
        "${QPCPP_DIR}/include"
        "${QPCPP_DIR}/src"
        "${QPCPP_DIR}/ports/esp-idf"
    REQUIRES freertos
)

