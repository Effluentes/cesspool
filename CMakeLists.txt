cmake_minimum_required(VERSION 3.22)

include(CMakeDependentOption)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds not allowed. Please make a build directory.")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


include(FetchContent)

FetchContent_Declare(
    qpcpp
    GIT_REPOSITORY https://github.com/QuantumLeaps/qpcpp.git
    GIT_TAG        v8.1.2
)

FetchContent_GetProperties(qpcpp)
if(NOT qpcpp_POPULATED)
    FetchContent_Populate(qpcpp)
endif()

set(QPCPP_SRC_DIR ${qpcpp_SOURCE_DIR} CACHE PATH "" FORCE)

set(NODE_TYPE master)
option(BUILD_MASTER_NODE "Build the master node" ON)
if(NOT BUILD_MASTER_NODE)
  set(NODE_TYPE slave)
endif()
set(MAIN_DIR ${CMAKE_SOURCE_DIR}/cesspool/${NODE_TYPE})
message(STATUS "building for: ${MAIN_DIR}")
set(EXTRA_COMPONENT_DIRS ${MAIN_DIR} ${CMAKE_SOURCE_DIR}/components)
set(EXTRA_COMPONENTS_DIRS  ${ADDITIONAL_PACKAGES})

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(
  cesspool
  VERSION 0.1.0
  LANGUAGES CXX)


add_subdirectory(${CMAKE_SOURCE_DIR}/packages)
# include(fetch_content/qpcpp)
