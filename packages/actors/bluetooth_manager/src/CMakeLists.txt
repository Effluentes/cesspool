project(${PACKAGE_NAME}_${MODULE_NAME})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE)
add_library(${PROJECT_NAME} ${SOURCE})
add_library(${PACKAGE_NAME}::${MODULE_NAME} ALIAS ${PROJECT_NAME})
set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
  # VERSION ${PROJECT_VERSION}
            #  SOVERSION ${PROJECT_VERSION_MAJOR}
            OUTPUT_NAME "${PACKAGE_NAME}-${PROJECT_NAME}-${MODULE_NAME}"
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
            CXX_STANDARD 23
            CXX_EXTENSIONS OFF
            CXX_STANDARD_REQUIRED ON
             )

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/
    SYSTEM3
        ${QPCPP_DIR}/include
        ${QPCPP_DIR}/ports/esp-idf

)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        actors::active_object
)