message(STATUS "ESP component ${COMPONENT_NAME}")

if(NOT DEFINED QPCPP_DIR OR QPCPP_DIR STREQUAL "")
    message(FATAL_ERROR "QPCPP_DIR is not set. Ensure FetchContent for qpcpp runs before project().")
endif()

idf_component_register(
    SRCS
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
        "${QPCPP_DIR}/include"
        "${QPCPP_DIR}/src"
        "${QPCPP_DIR}/ports/esp-idf"
    REQUIRES freertos
)