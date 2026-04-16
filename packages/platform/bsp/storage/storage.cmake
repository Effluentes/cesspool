project(
  ${PACKAGE_NAME}_${MODULE_NAME}_${SUBMODULE_NAME}
  VERSION 0.1.0)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PACKAGE_NAME}::${MODULE_NAME}::${SUBMODULE_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
                      INTERFACE ${PACKAGE_NAME}::${MODULE_NAME}::${SUBMODULE_NAME}::storage
                      INTERFACE ${PACKAGE_NAME}::${MODULE_NAME}::${SUBMODULE_NAME}::nvs
                        )