message(STATUS "ESP component ${COMPONENT_NAME}")

idf_component_register(
    SRCS
        src/ScreenManagerAO.cc
    INCLUDE_DIRS
        include
    REQUIRES
        esp_driver_gpio
        qpcpp
        platform_driver_esp32
)

