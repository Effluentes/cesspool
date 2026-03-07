idf_component_register(SRCS
					   INCLUDE_DIRS
					   INCLUDE_DIRS
					   EMBED_FILES
					   )

# target_link_libraries(${COMPONENT_TARGET} INTERFACE platform::storage::storage platform::storage::nvs)
target_link_libraries(${COMPONENT_TARGET} INTERFACE platform::bsp::startup)
message(STATUS "ESP component ${COMPONENT_NAME}")