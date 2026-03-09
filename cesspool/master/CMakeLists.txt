set(SOURCE ./src/main.cc)

idf_component_register(SRCS ${SOURCE}
                    #    REQUIRES freertos driver esp_timer communication nvs_flash bt platform)
                       REQUIRES freertos platform actors)

message("#TEST: ${COMPONENT_TARGET}")