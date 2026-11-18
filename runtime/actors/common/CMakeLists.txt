set(NAME ${PACKAGE_NAME}_${MODULE_NAME})
add_library(${NAME} INTERFACE)
add_library(${PACKAGE_NAME}::${MODULE_NAME} ALIAS ${NAME})

target_include_directories(${NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${NAME} INTERFACE qpcpp)
