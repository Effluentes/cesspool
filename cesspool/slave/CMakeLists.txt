set(SOURCE ./src/main.cc)

idf_component_register(SRCS ${SOURCE}
                       REQUIRES freertos driver esp_timer)