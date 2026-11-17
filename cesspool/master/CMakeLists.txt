message(STATUS "ESP component: ${COMPONENT_NAME}")
set(SOURCE
    ./src/main.cc
    ${CMAKE_SOURCE_DIR}/packages/configuration/configuration/src/ConfigurationRecord.cc
    ${CMAKE_SOURCE_DIR}/packages/configuration/configuration/src/ConfigurationRepository.cc
    ${CMAKE_SOURCE_DIR}/platforms/esp32/configuration/configuration/src/ConfigurationLoader.cc
    ${CMAKE_SOURCE_DIR}/platforms/esp32/human_machine_interface/controls/src/Controls.cc
    ${CMAKE_SOURCE_DIR}/platforms/esp32/human_machine_interface/display/src/Display.cc
    ${CMAKE_SOURCE_DIR}/platforms/esp32/observability/src/Esp32Logger.cc)

idf_component_register(
  SRCS ${SOURCE}
  INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/platforms/esp32/configuration/configuration/include
    ${CMAKE_SOURCE_DIR}/platforms/esp32/human_machine_interface/controls/include
    ${CMAKE_SOURCE_DIR}/platforms/esp32/human_machine_interface/display/include
    ${CMAKE_SOURCE_DIR}/platforms/esp32/observability/include
    ${CMAKE_SOURCE_DIR}/packages/configuration/configuration/include
  REQUIRES u8g2 esp_driver_gpio esp_system freertos log nvs_flash)

idf_component_get_property(U8G2_COMPONENT_LIB nixy4__u8g2 COMPONENT_LIB)
target_link_libraries(${U8G2_COMPONENT_LIB} PUBLIC idf::esp_driver_gpio)

add_dependencies(${COMPONENT_LIB} composite_root::app composite_root::system_orchestrator actors::system_ao qpcpp)
target_link_libraries(${COMPONENT_LIB} PRIVATE composite_root::app composite_root::system_orchestrator
                                               actors::system_ao configuration::configuration qpcpp)
# ESP-IDF's RISC-V headers use GNU statement expressions; keep these pedantic diagnostics visible,
# but do not let vendor-header extensions fail the application component build.
target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Wpedantic -Werror
                                                 -Wno-error=pedantic)
