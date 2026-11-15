add_library(actors_common INTERFACE)
add_library(actors::common ALIAS actors_common)

target_include_directories(actors_common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(actors_common INTERFACE qpcpp)
