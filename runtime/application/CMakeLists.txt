add_library(runtime_application src/Application.cc)
add_library(runtime::application ALIAS runtime_application)

target_include_directories(
  runtime_application
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(
  runtime_application
  PROPERTIES CXX_STANDARD 23
             CXX_EXTENSIONS OFF
             CXX_STANDARD_REQUIRED ON)

target_compile_options(runtime_application
  PRIVATE -Wall -Wextra -Wpedantic -Werror)
