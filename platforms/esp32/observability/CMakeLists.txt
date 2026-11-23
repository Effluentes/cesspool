idf_component_register(
  SRCS src/Esp32Logger.cc
  INCLUDE_DIRS include ${CMAKE_SOURCE_DIR}/packages/observability/logs/include
  REQUIRES log)
