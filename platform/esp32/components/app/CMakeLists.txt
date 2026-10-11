message(STATUS "ESP component ${COMPONENT_NAME}")

idf_component_register(
    SRCS
        "src/App.cc"
    INCLUDE_DIRS "include"
        "${CMAKE_SOURCE_DIR}/packages/domain/include"
        "${CMAKE_SOURCE_DIR}/packages/ports/include"
        "${CMAKE_SOURCE_DIR}/packages/events/include"
        "${CMAKE_SOURCE_DIR}/runtime/actors/include"
        "${CMAKE_SOURCE_DIR}/runtime/composite_root/include"
    REQUIRES qpcpp freertos driver bt nvs_flash
    REQUIRES platform_driver_esp32 bluetooth_nimble_stack_inicializator
    REQUIRES bluetooth_port_esp32
)