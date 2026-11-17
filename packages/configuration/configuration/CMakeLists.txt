add_library(configuration INTERFACE)
add_library(configuration::configuration ALIAS configuration)

target_include_directories(configuration
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(configuration INTERFACE cxx_std_23)
target_compile_options(configuration
  INTERFACE -Wall -Wextra -Wpedantic -Werror)
