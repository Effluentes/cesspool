cmake_minimum_required(VERSION 3.22)

set(NODE_TYPE master)
option(BUILD_MASTER_NODE "Build the master node" ON)
if(NOT BUILD_MASTER_NODE)
  set(NODE_TYPE slave)
endif()
set(MAIN_DIR ${CMAKE_SOURCE_DIR}/cesspool/${NODE_TYPE})
message(STATUS "building for: ${MAIN_DIR}")

include(FetchContent)

FetchContent_Declare(
    qpcpp
    GIT_REPOSITORY https://github.com/vChavezB/qpcpp.git
    GIT_TAG        origin/esp32
)

FetchContent_Populate(qpcpp)
set(QPCPP_DIR ${qpcpp_SOURCE_DIR} CACHE INTERNAL "Path to fetched qpcpp")
message(STATUS "QPCPP_DIR: ${QPCPP_DIR}")

set(EXTRA_COMPONENT_DIRS ${MAIN_DIR} ${CMAKE_SOURCE_DIR}/third_party ${CMAKE_SOURCE_DIR}/components)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)


project(cesspool)

add_subdirectory(${CMAKE_SOURCE_DIR}/packages)