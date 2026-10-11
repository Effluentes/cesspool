message(STATUS "Cesspool domain layer")

add_library(cesspool_domain INTERFACE)
target_include_directories(cesspool_domain INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(cesspool_domain PROPERTIES
	CXX_STANDARD 23
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF)
