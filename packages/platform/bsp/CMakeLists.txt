set(MODULE_NAME "bsp")

add_subdirectory(startup_result)

add_library(platform_bsp_storage INTERFACE)
add_library(platform::bsp::storage ALIAS platform_bsp_storage)