message(STATUS "ESP component: ${COMPONENT_NAME}")
set(SOURCE ./src/main.cc)

idf_component_register(SRCS ${SOURCE} REQUIRES)

add_dependencies(${COMPONENT_LIB} composite_root::app composite_root::system_orchestrator actors::system_ao qpcpp)
target_link_libraries(${COMPONENT_LIB} PRIVATE composite_root::app composite_root::system_orchestrator
                                               actors::system_ao qpcpp)
