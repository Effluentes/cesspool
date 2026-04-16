message(STATUS "ESP component ${COMPONENT_NAME}")

idf_component_register(
    REQUIRES freertos actors
)
