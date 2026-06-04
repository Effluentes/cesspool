message(STATUS "ESP component ${COMPONENT_NAME}")

idf_component_register(
    SRCS "src/Blinker.cc"
    INCLUDE_DIRS "include"
    REQUIRES qpcpp freertos driver
)