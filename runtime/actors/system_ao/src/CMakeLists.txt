set(NAME ${PACKAGE_NAME}_${MODULE_NAME})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE)
add_library(${NAME} ${SOURCE})
add_library(${PACKAGE_NAME}::${MODULE_NAME} ALIAS ${NAME})
set_target_properties(
  ${NAME}
  PROPERTIES
            OUTPUT_NAME "${PACKAGE_NAME}-${MODULE_NAME}"
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
            CXX_STANDARD 23
            CXX_EXTENSIONS OFF
            CXX_STANDARD_REQUIRED ON
            )

target_include_directories(${NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/
)

target_link_libraries(${NAME}
PRIVATE
qpcpp
)


target_compile_options(
  ${NAME}
  PRIVATE $<$<CXX_COMPILER_ID:CXX,GNU>:$<$<CONFIG:Release>:${GCC_FLAGS_RELEASE}>>
  PRIVATE $<$<CXX_COMPILER_ID:CXX,GNU>:$<$<CONFIG:Debug>:${GCC_FLAGS_DEBUG}>>
)
