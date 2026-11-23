idf_component_register(
  SRCS src/Display.cc
  INCLUDE_DIRS include ${CMAKE_SOURCE_DIR}/packages/human_machine_interface/display/include
  PRIV_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/managed_components/nixy4__u8g2/src/port
  REQUIRES u8g2 esp_driver_gpio esp_driver_i2c)
