set(MODULE_NAME "driver")

add_subdirectory(gpio)
add_subdirectory(i2c)
add_subdirectory(display)

message(STATUS "Platform driver module ${MODULE_NAME}")