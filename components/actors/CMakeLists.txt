idf_component_register(
    SRCS
        "${QPCPP_SRC_DIR}/src/qf/qep_hsm.cpp"
        "${QPCPP_SRC_DIR}/src/qf/qep_msm.cpp"
        "${QPCPP_SRC_DIR}/src/qf/qf_act.cpp"
        "${QPCPP_SRC_DIR}/src/qf/qf_actq.cpp"
        "${QPCPP_SRC_DIR}/src/qf/qf_defer.cpp"
        "${QPCPP_SRC_DIR}/src/qf/qf_dyn.cpp"
        "${QPCPP_SRC_DIR}/src/qf/qf_mem.cpp"
        "${QPCPP_SRC_DIR}/src/qf/qf_ps.cpp"
        "${QPCPP_SRC_DIR}/src/qf/qf_qact.cpp"
        "${QPCPP_SRC_DIR}/src/qf/qf_qeq.cpp"
        "${QPCPP_SRC_DIR}/src/qf/qf_qmact.cpp"
        "${QPCPP_SRC_DIR}/src/qf/qf_time.cpp"
        "${QPCPP_SRC_DIR}/ports/freertos/qf_port.cpp"
    INCLUDE_DIRS
        "."
        "${QPCPP_SRC_DIR}/include"
        "${QPCPP_SRC_DIR}/src"
        "${QPCPP_SRC_DIR}/ports/freertos"
        "${QPCPP_SRC_DIR}/ports/config"
		"$ENV{IDF_PATH}/components/freertos/FreeRTOS-Kernel/include/freertos"
        "$ENV{IDF_PATH}/components/freertos/config/include/freertos"
        "$ENV{IDF_PATH}/components/freertos/esp_additions/include/freertos"
    REQUIRES freertos
)
# target_link_libraries(${COMPONENT_TARGET} INTERFACE actors::bluetooth)
target_link_libraries(${COMPONENT_TARGET} INTERFACE actors::system)
target_include_directories(${COMPONENT_TARGET} SYSTEM INTERFACE
    $ENV{IDF_PATH}/components/freertos/FreeRTOS-Kernel/include/freertos
    $ENV{IDF_PATH}/components/freertos/config/include/freertos
    $ENV{IDF_PATH}/components/freertos/esp_additions/include/freertos
)
message(STATUS "ESP component ${COMPONENT_NAME}")