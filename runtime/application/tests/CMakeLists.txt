cmake_minimum_required(VERSION 3.22)
project(cesspool_application_tests LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG 2b60af89e23d28eefc081bc930831ee9d45ea58b)
FetchContent_MakeAvailable(Catch2)

enable_testing()

add_executable(application_tests
  application_tests.cc
  configuration_tests.cc
  ../../../packages/configuration/configuration/src/ConfigurationRecord.cc
  ../../../packages/configuration/configuration/src/ConfigurationRepository.cc
  ../src/Application.cc)
target_include_directories(
  application_tests
  PRIVATE ../include
          ../../../packages/configuration/configuration/include)
target_link_libraries(application_tests PRIVATE Catch2::Catch2WithMain)
target_compile_features(application_tests PRIVATE cxx_std_23)
set_target_properties(application_tests PROPERTIES CXX_EXTENSIONS OFF)

include(Catch)
catch_discover_tests(application_tests)

add_executable(application_allocation_tests
  allocation_tests.cc
  ../src/Application.cc)
target_include_directories(application_allocation_tests PRIVATE ../include)
target_compile_features(application_allocation_tests PRIVATE cxx_std_23)
target_link_options(application_allocation_tests PRIVATE
  -Wl,--wrap=malloc
  -Wl,--wrap=calloc
  -Wl,--wrap=realloc)
add_test(NAME application-runtime-no-allocations COMMAND application_allocation_tests)

option(CESSPOOL_BUILD_FUZZERS "Build libFuzzer targets" OFF)
if(CESSPOOL_BUILD_FUZZERS)
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR "CESSPOOL_BUILD_FUZZERS requires Clang with libFuzzer support")
  endif()

  add_executable(configuration_record_fuzzer
    configuration_record_fuzzer.cc
    ../../../packages/configuration/configuration/src/ConfigurationRecord.cc)
  target_include_directories(configuration_record_fuzzer
    PRIVATE ../../../packages/configuration/configuration/include)
  target_compile_features(configuration_record_fuzzer PRIVATE cxx_std_23)
  target_compile_options(configuration_record_fuzzer
    PRIVATE -fsanitize=fuzzer,address,undefined -fno-omit-frame-pointer
            -D__cpp_concepts=202002L -Wno-builtin-macro-redefined)
  target_link_options(configuration_record_fuzzer
    PRIVATE -fsanitize=fuzzer,address,undefined)
endif()
