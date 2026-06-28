message(STATUS "ESP component ${COMPONENT_NAME}")
set(SOURCE ./src/main.cc)

idf_component_register(SRCS ${SOURCE}
                       REQUIRES app driver u8g2)
