message(STATUS "ESP component ${COMPONENT_NAME}")

idf_component_register(
    SRCS
        "src/App.cc"
    INCLUDE_DIRS "include"
    REQUIRES qpcpp freertos driver bt nvs_flash
    REQUIRES platform_driver_esp32 bluetooth_nimble_stack_inicializator
    REQUIRES blackboard_system_state_ao bluetooth_manager_ao screen_manager_ao
)