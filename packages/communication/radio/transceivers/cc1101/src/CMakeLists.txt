project(
  ${MODULE_NAME}_${COMPONENT_NAME}_${SUBCOMPONENT_NAME}
  LANGUAGES CXX)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE)
add_library(${PROJECT_NAME} ${SOURCE})
add_library(${CMAKE_PROJECT_NAME}::${PACKAGE_NAME}::${MODULE_NAME}::${COMPONENT_NAME} ALIAS ${PROJECT_NAME})
message(STATUS "Added component: ${CMAKE_PROJECT_NAME}::${PACKAGE_NAME}::${MODULE_NAME}::${COMPONENT_NAME}")
set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
  # VERSION ${PROJECT_VERSION}
            #  SOVERSION ${PROJECT_VERSION_MAJOR}
            #  OUTPUT_NAME "${PACKAGE_NAME}-${COMPONENT_NAME}-${MODULE_NAME}"
            #  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
            #  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
            #  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
            #  CXX_STANDARD 23
             CXX_EXTENSIONS OFF
            #  CXX_STANDARD_REQUIRED ON
             )

# add_logger(${PROJECT_NAME})

# idf_component_register(
#     SRCS "${SOURCE}"
#     INCLUDE_DIRS "../include"
#     REQUIRES driver   # jeśli używasz np. SPI z ESP-IDF
# )

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES
        "${CMAKE_CURRENT_SOURCE_DIR}/include;${CMAKE_SOURCE_DIR}/packages"
)

target_compile_options(
  ${PROJECT_NAME}
  PRIVATE $<$<CXX_COMPILER_ID:CXX,GNU>:$<$<CONFIG:Release>:${GCC_FLAGS_RELEASE}>>
  PRIVATE $<$<CXX_COMPILER_ID:CXX,GNU>:$<$<CONFIG:Debug>:${GCC_FLAGS_DEBUG}>>)