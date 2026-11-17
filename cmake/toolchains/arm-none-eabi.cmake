set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

find_program(ARM_NONE_EABI_CXX arm-none-eabi-g++)
if(NOT ARM_NONE_EABI_CXX)
  message(FATAL_ERROR "arm-none-eabi-g++ is required for the STM32 compile-only preset")
endif()

set(CMAKE_CXX_COMPILER "${ARM_NONE_EABI_CXX}" CACHE FILEPATH "ARM C++ compiler")
set(CMAKE_CXX_FLAGS_INIT "-mcpu=cortex-m4 -mthumb -ffreestanding")
