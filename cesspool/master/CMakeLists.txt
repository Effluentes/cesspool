message(STATUS "ESP component ${COMPONENT_NAME}")
set(SOURCE ./src/main.cc)

idf_component_register(SRCS ${SOURCE}
                       REQUIRES  actors platform  esp_timer)
