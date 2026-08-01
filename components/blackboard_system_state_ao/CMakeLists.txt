message(STATUS "ESP component ${COMPONENT_NAME}")

idf_component_register(
    SRCS
        "src/SystemStateAO.cc"
    INCLUDE_DIRS "include"
    REQUIRES qpcpp
)