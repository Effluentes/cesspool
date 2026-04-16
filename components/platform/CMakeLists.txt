message(STATUS "ESP component ${COMPONENT_NAME}")
idf_component_register(	SRCS
						INCLUDE_DIRS
						INCLUDE_DIRS
						EMBED_FILES
    					PRIV_REQUIRES nvs_flash)

target_link_libraries(${COMPONENT_TARGET} INTERFACE platform::bsp::startup)
target_link_libraries(${COMPONENT_TARGET} INTERFACE platform::bsp::storage)