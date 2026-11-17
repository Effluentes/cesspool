cmake_minimum_required(VERSION 3.22)

option(CESSPOOL_HOST_TESTS "Configure host-only tests" OFF)
option(CESSPOOL_STM32_COMPILE_ONLY "Compile the target-neutral STM32 skeleton" OFF)

if(CESSPOOL_HOST_TESTS AND CESSPOOL_STM32_COMPILE_ONLY)
  message(FATAL_ERROR "Host tests and STM32 compile-only modes are mutually exclusive")
elseif(CESSPOOL_HOST_TESTS)
  project(cesspool_host LANGUAGES CXX)
  enable_testing()
  add_subdirectory(runtime/application/tests)
  return()
elseif(CESSPOOL_STM32_COMPILE_ONLY)
  project(cesspool_stm32 LANGUAGES CXX)
  add_subdirectory(platforms/stm32)
  return()
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(composite_root)
include(add_package)
include(add_module)

add_subdirectory(${CMAKE_SOURCE_DIR}/packages)

# --------------------------------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
  qpcpp
  GIT_REPOSITORY https://github.com/vChavezB/qpcpp.git
  GIT_TAG 435131fdb5ffe5cb383aa16b5cf596a1b462167a)
FetchContent_MakeAvailable(qpcpp)
file(READ "${qpcpp_SOURCE_DIR}/ports/esp-idf/qf_port.cpp" QPCPP_PORT_SOURCE)
string(FIND "${QPCPP_PORT_SOURCE}" "xTaskCreateStaticPinnedToCore(" QPCPP_STATIC_TASK_API)
if(QPCPP_STATIC_TASK_API EQUAL -1)
  find_package(Git REQUIRED)
  execute_process(
    COMMAND "${GIT_EXECUTABLE}" apply --check
            "${CMAKE_SOURCE_DIR}/cmake/patches/qpcpp-static-task-allocation.patch"
    WORKING_DIRECTORY "${qpcpp_SOURCE_DIR}"
    RESULT_VARIABLE QPCPP_PATCH_CHECK
    OUTPUT_QUIET
    ERROR_VARIABLE QPCPP_PATCH_ERROR)
  if(NOT QPCPP_PATCH_CHECK EQUAL 0)
    message(FATAL_ERROR "Cannot apply the pinned qpcpp static-task patch: ${QPCPP_PATCH_ERROR}")
  endif()
  execute_process(
    COMMAND "${GIT_EXECUTABLE}" apply
            "${CMAKE_SOURCE_DIR}/cmake/patches/qpcpp-static-task-allocation.patch"
    WORKING_DIRECTORY "${qpcpp_SOURCE_DIR}"
    RESULT_VARIABLE QPCPP_PATCH_RESULT
    ERROR_VARIABLE QPCPP_PATCH_ERROR)
  if(NOT QPCPP_PATCH_RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to apply the pinned qpcpp static-task patch: ${QPCPP_PATCH_ERROR}")
  endif()
endif()
set(QPCPP_src
    "${qpcpp_SOURCE_DIR}/src/qf/qep_hsm.cpp"
    "${qpcpp_SOURCE_DIR}/src/qf/qep_msm.cpp"
    "${qpcpp_SOURCE_DIR}/src/qf/qf_act.cpp"
    "${qpcpp_SOURCE_DIR}/src/qf/qf_actq.cpp"
    "${qpcpp_SOURCE_DIR}/src/qf/qf_defer.cpp"
    "${qpcpp_SOURCE_DIR}/src/qf/qf_dyn.cpp"
    "${qpcpp_SOURCE_DIR}/src/qf/qf_mem.cpp"
    "${qpcpp_SOURCE_DIR}/src/qf/qf_ps.cpp"
    "${qpcpp_SOURCE_DIR}/src/qf/qf_qact.cpp"
    "${qpcpp_SOURCE_DIR}/src/qf/qf_qeq.cpp"
    "${qpcpp_SOURCE_DIR}/src/qf/qf_qmact.cpp"
    "${qpcpp_SOURCE_DIR}/src/qf/qf_time.cpp"
    "${qpcpp_SOURCE_DIR}/ports/esp-idf/qf_port.cpp")
add_library(qpcpp STATIC ${QPCPP_src})

target_include_directories(qpcpp PUBLIC ${qpcpp_SOURCE_DIR}/include "${qpcpp_SOURCE_DIR}/src"
                                        ${qpcpp_SOURCE_DIR}/ports/esp-idf)
target_compile_definitions(qpcpp PRIVATE "PRO_CPU_NUM=0" "APP_CPU_NUM=1")
target_compile_options(qpcpp PRIVATE "-include" "freertos/idf_additions.h")

target_link_libraries(qpcpp PUBLIC idf::freertos)
# --------------------------------------------------------------------------------------------------

# todo(miekicki): change to real checking a platform and choose a right port for board/plaform
set(ESP_32 TRUE)
if(ESP_32)
  set(EXTRA_COMPONENT_DIRS ${MAIN_DIR} # ${CMAKE_SOURCE_DIR}/platform/esp32/third_party
      # ${CMAKE_SOURCE_DIR}/platform/esp32/components
  )
  include($ENV{IDF_PATH}/tools/cmake/project.cmake)
  project(cesspool)
else()
  message(FATAL_ERROR "This project is intended to be built for ESP32 only. Please set ESP_32 to true.")
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/runtime)
