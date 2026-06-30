message(STATUS "ESP component ${COMPONENT_NAME}")

idf_component_register(
    SRCS
        "src/App.cc"
    INCLUDE_DIRS "include"
    REQUIRES qpcpp freertos driver bt nvs_flash
    REQUIRES bluetooth_nimble_stack_inicializator platform_driver_esp32 bluetooth_manager_ao nvs_flash
)