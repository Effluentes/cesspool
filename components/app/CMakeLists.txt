message(STATUS "ESP component ${COMPONENT_NAME}")

    idf_component_register(
        SRCS
            "src/App.cc"
        INCLUDE_DIRS "include"
        REQUIRES qpcpp freertos driver bt nvs_flash
        REQUIRES radio_ao bluetooth_nimble_stack_inicializator platform_driver_esp32
    )

