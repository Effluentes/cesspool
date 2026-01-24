idf_component_register(SRCS
					   INCLUDE_DIRS
					   INCLUDE_DIRS
					   EMBED_FILES
					   )

target_link_libraries(${COMPONENT_TARGET} INTERFACE communication::radio::transceivers::cc1101)
message(STATUS "ESP component ${COMPONENT_NAME}")