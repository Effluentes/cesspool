message(STATUS "ESP component ${COMPONENT_NAME}")

idf_component_register(
    SRCS
    INCLUDE_DIRS
    REQUIRES freertos
)

target_link_libraries(${COMPONENT_TARGET} INTERFACE my_qpcpp)
