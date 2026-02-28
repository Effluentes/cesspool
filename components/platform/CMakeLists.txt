idf_component_register(SRCS
					   INCLUDE_DIRS
					   INCLUDE_DIRS
					   EMBED_FILES
					   REQUIRES nvs_flash
					   )

target_link_libraries(${COMPONENT_TARGET} INTERFACE platform::storage::storage platform::storage::nvs)
message(STATUS "ESP component ${COMPONENT_NAME}")