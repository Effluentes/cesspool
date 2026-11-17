include(FetchContent)
FetchContent_Declare(
  qpcpp
  GIT_REPOSITORY https://github.com/vChavezB/qpcpp.git
  GIT_TAG 435131fdb5ffe5cb383aa16b5cf596a1b462167a)
set(QPCPP_CFG_PORT
    "posix"
    CACHE STRING "" FORCE)
FetchContent_MakeAvailable(qpcpp)

FetchContent_GetProperties(qpcpp)
if(NOT qpcpp_POPULATED)
  FetchContent_Populate(qpcpp)
  add_subdirectory(${qpcpp_SOURCE_DIR} ${qpcpp_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
