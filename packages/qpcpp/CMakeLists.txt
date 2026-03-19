project(my_qpcpp)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE)
message(STATUS "my_qpcpp CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
add_library(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/QPEspIdfPort.cc)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${QPCPP_DIR}/include
        ${QPCPP_DIR}/ports/freertos
)


target_compile_options(
  ${PROJECT_NAME}
  PRIVATE $<$<CXX_COMPILER_ID:CXX,GNU>:$<$<CONFIG:Release>:${GCC_FLAGS_RELEASE}>>
  PRIVATE $<$<CXX_COMPILER_ID:CXX,GNU>:$<$<CONFIG:Debug>:${GCC_FLAGS_DEBUG}>>)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        idf::freertos)