message(STATUS "ESP component ${COMPONENT_NAME}")

message(STATUS "CMAKE_BUILD_EARLY_EXPANSION is ON")

idf_component_register(
    SRCS
        src/GPIO.cc
        src/I2C.cc
        src/EncoderDriver.cc
        src/Sh1106.cc
    INCLUDE_DIRS
        include
    REQUIRES
        esp_driver_gpio
        qpcpp
)

target_link_libraries(${COMPONENT_TARGET} PUBLIC platform::driver::gpio platform::driver::i2c)
