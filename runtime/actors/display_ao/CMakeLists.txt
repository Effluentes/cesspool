cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME MODULE_NAME)
string(REPLACE " " "_" ProjectId ${MODULE_NAME})
message(STATUS "Adding module: ${MODULE_NAME}" for package ${PACKAGE_NAME})

add_subdirectory(src)
