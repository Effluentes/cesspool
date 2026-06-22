message(STATUS "ESP component ${COMPONENT_NAME}")


idf_component_register(
    SRCS "src/BluetoothNimbleStackInicializator.cc"
    INCLUDE_DIRS "include"
    REQUIRES qpcpp freertos driver bt nvs_flash
)
if(NOT CMAKE_BUILD_EARLY_EXPANSION)
target_link_libraries(${COMPONENT_TARGET} PUBLIC platform::bsp::startup_result)

endif()