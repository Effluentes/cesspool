add_library(cesspool_ports INTERFACE)
target_include_directories(cesspool_ports INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(cesspool_ports INTERFACE cesspool_domain)
set_target_properties(cesspool_ports PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)
