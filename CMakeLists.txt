cmake_minimum_required(VERSION 3.22)
project(cesspool)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(composite_root)

include(FetchContent)

FetchContent_Declare(
    qpcpp
    GIT_REPOSITORY https://github.com/vChavezB/qpcpp.git
    GIT_TAG        origin/esp32
)

FetchContent_Populate(qpcpp)
set(QPCPP_DIR ${qpcpp_SOURCE_DIR} CACHE INTERNAL "Path to fetched qpcpp")
message(STATUS "QPCPP_DIR: ${QPCPP_DIR}")

# todo(miekicki): change to real checking a platform and choose a right port for board/plaform
set(ESP_32 TRUE)
if(ESP_32)
  set(EXTRA_COMPONENT_DIRS
      ${MAIN_DIR}
      ${CMAKE_SOURCE_DIR}/runtime/active_objects
      ${CMAKE_SOURCE_DIR}/platform/esp32/third_party
      ${CMAKE_SOURCE_DIR}/platform/esp32/components
  )
  include($ENV{IDF_PATH}/tools/cmake/project.cmake)
else()
  message(FATAL_ERROR "This project is intended to be built for ESP32 only. Please set ESP_32 to true.")
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/packages)
