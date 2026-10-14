function(add_module module_name)
  add_subdirectory(${module_name})
  message(STATUS "Adding module: ${module_name}")
endfunction()
