message(STATUS "ESP component: ${COMPONENT_NAME}")
set(SOURCE
    ./src/main.cc
    ${CMAKE_SOURCE_DIR}/platforms/esp32/human_machine_interface/display/src/Display.cc)

idf_component_register(
  SRCS ${SOURCE}
  INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/platforms/esp32/human_machine_interface/controls/include
    ${CMAKE_SOURCE_DIR}/platforms/esp32/human_machine_interface/display/include
    ${CMAKE_SOURCE_DIR}/packages/human_machine_interface/display/include
  REQUIRES u8g2 esp_driver_gpio freertos)

idf_component_get_property(U8G2_COMPONENT_LIB nixy4__u8g2 COMPONENT_LIB)
target_link_libraries(${U8G2_COMPONENT_LIB} PUBLIC idf::esp_driver_gpio)

add_dependencies(${COMPONENT_LIB} composite_root::app composite_root::system_orchestrator actors::system_ao qpcpp)
target_link_libraries(${COMPONENT_LIB} PRIVATE composite_root::app composite_root::system_orchestrator
                                               actors::system_ao qpcpp)
