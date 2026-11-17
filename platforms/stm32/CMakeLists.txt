cmake_minimum_required(VERSION 3.22)
project(cesspool_stm32_compile LANGUAGES CXX)

add_library(stm32_application OBJECT
  ../../runtime/application/src/Application.cc
  ../../packages/configuration/configuration/src/ConfigurationRecord.cc
  ../../packages/configuration/configuration/src/ConfigurationRepository.cc)

target_include_directories(stm32_application
  PRIVATE ../../runtime/application/include
          ../../packages/configuration/configuration/include)
target_compile_features(stm32_application PRIVATE cxx_std_23)
target_compile_options(stm32_application
  PRIVATE -Wall -Wextra -Wpedantic -Werror)
