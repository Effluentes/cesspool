cmake_minimum_required(VERSION 3.22)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(composite_root)
include(add_package)
include(add_module)


#--------------------------------------------------------------------------------------------------
include(FetchContent)

# 1. Pobieranie repozytorium (jeśli nie zostało pobrane wcześniej)
FetchContent_Declare(
    qpcpp
    GIT_REPOSITORY https://github.com/vChavezB/qpcpp.git
    GIT_TAG        origin/esp32
)
FetchContent_MakeAvailable(qpcpp)

# 2. Automatyczne wyszukanie właściwych plików .cpp bez zgadywania ścieżek
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
    "${qpcpp_SOURCE_DIR}/ports/esp-idf/qf_port.cpp"
)
# 3. Tworzenie właściwej biblioteki statycznej QP/C++
add_library(qpcpp STATIC ${QPCPP_src})

# 4. Dodanie ścieżek do nagłówków (.hpp) widocznych publicznie
target_include_directories(qpcpp PUBLIC
    ${qpcpp_SOURCE_DIR}/include
    "${qpcpp_SOURCE_DIR}/src"
    ${qpcpp_SOURCE_DIR}/ports/esp-idf
)
#link free rtos to qpcpp
target_compile_definitions(qpcpp PRIVATE
    "PRO_CPU_NUM=0"
    "APP_CPU_NUM=1"
)
target_compile_options(qpcpp PRIVATE "-include" "freertos/idf_additions.h")

target_link_libraries(qpcpp PUBLIC idf::freertos)
#--------------------------------------------------------------------------------------------------

# todo(miekicki): change to real checking a platform and choose a right port for board/plaform
set(ESP_32 TRUE)
if(ESP_32)
  set(EXTRA_COMPONENT_DIRS
      ${MAIN_DIR}
  #     ${CMAKE_SOURCE_DIR}/platform/esp32/third_party
  #     ${CMAKE_SOURCE_DIR}/platform/esp32/components
  )
  include($ENV{IDF_PATH}/tools/cmake/project.cmake)
  project(cesspool)
else()
  message(FATAL_ERROR "This project is intended to be built for ESP32 only. Please set ESP_32 to true.")
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/packages)
add_subdirectory(${CMAKE_SOURCE_DIR}/runtime)

