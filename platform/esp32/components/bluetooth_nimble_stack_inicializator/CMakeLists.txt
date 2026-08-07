message(STATUS "ESP component ${COMPONENT_NAME}")


idf_component_register(
    SRCS "src/BluetoothNimbleStackInicializator.cc"
    INCLUDE_DIRS "include"
    REQUIRES qpcpp freertos driver bt nvs_flash
)
message(STATUS "ESP testttt ${COMPONENT_TARGET}")
target_link_libraries(${COMPONENT_TARGET} PUBLIC platform::bsp::startup_result)