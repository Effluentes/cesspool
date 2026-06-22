message(STATUS "ESP component ${COMPONENT_NAME}")

if(CMAKE_BUILD_EARLY_EXPANSION)
    message(STATUS "ESP component ${COMPONENT_NAME} - early expansion, registering with no sources")
    idf_component_register(
        PRIV_REQUIRES radio_ao bluetooth_nimble_stack_inicializator
    )
    return()
endif()

idf_component_register(
    SRCS
        "src/App.cc"
    INCLUDE_DIRS "include"
    REQUIRES qpcpp freertos driver bt nvs_flash
    PRIV_REQUIRES radio_ao bluetooth_nimble_stack_inicializator
)
