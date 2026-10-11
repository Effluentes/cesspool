add_library(cesspool_events INTERFACE)
target_include_directories(cesspool_events INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(cesspool_events INTERFACE cesspool_domain)
set_target_properties(cesspool_events PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)
