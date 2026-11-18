message(STATUS "ESP component: ${COMPONENT_NAME}")
set(SOURCE
    ./src/main.cc
)

idf_component_register(
  SRCS ${SOURCE}
  REQUIRES u8g2 esp_driver_gpio freertos)

add_dependencies(${COMPONENT_LIB} esp32::configuration::configuration composite_root::app composite_root::system_orchestrator actors::system_ao qpcpp)
target_link_libraries(${COMPONENT_LIB} PRIVATE esp32::configuration::configuration composite_root::app composite_root::system_orchestrator
                                               actors::system_ao qpcpp)
