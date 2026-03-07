project(${PACKAGE_NAME}_${MODULE_NAME}_${SUBMODULE_NAME}_${COMPONENT_NAME})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE)
add_library(${PROJECT_NAME} ${SOURCE})
add_library(${PACKAGE_NAME}::${MODULE_NAME}::${SUBMODULE_NAME}::${COMPONENT_NAME} ALIAS ${PROJECT_NAME})
set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
  # VERSION ${PROJECT_VERSION}
            #  SOVERSION ${PROJECT_VERSION_MAJOR}
            OUTPUT_NAME "${PACKAGE_NAME}-${PROJECT_NAME}-${MODULE_NAME}"
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
            CXX_STANDARD 23
            CXX_EXTENSIONS OFF
            CXX_STANDARD_REQUIRED ON
             )

# add_logger(${PROJECT_NAME})

# target_include_directories(${PROJECT_NAME}
#     PUBLIC
#         ${CMAKE_SOURCE_DIR}/packages
# )

#target_include_directories(${PROJECT_NAME}
#    PUBLIC
#        # Dla plików w tym samym komponencie
#        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
#        # Dla użytkowników biblioteki - stworzymy wirtualną strukturę
#        $<INSTALL_INTERFACE:include/communication/radio/transceivers/cc1101>
#)
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        platform::bsp::startup_result
        platform::bsp::storage::storage
        idf::nvs_flash
)

target_compile_options(
  ${PROJECT_NAME}
  PRIVATE $<$<CXX_COMPILER_ID:CXX,GNU>:$<$<CONFIG:Release>:${GCC_FLAGS_RELEASE}>>
  PRIVATE $<$<CXX_COMPILER_ID:CXX,GNU>:$<$<CONFIG:Debug>:${GCC_FLAGS_DEBUG}>>)

