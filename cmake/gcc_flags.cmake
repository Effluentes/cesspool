set(GCC_FLAGS
    -Werror
    -Wall
    -Wextra
    -Wpedantic
    -Wduplicated-cond
    -Wduplicated-branches
    -Wformat=2
    -Weffc++
    -Wdouble-promotion
    -Wuseless-cast
    -Wnull-dereference
    -Wlogical-op
    -Wmisleading-indentation
    -Wsign-conversion
    -Wconversion
    -Woverloaded-virtual
    -Wunused
    -Wshadow
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wcast-align
    -Wundef
    -Wpointer-arith
    -Wstrict-overflow=2
    -Wswitch-default
    -Wswitch-enum
    -Wunreachable-code)

set(GCC_FLAGS_DEBUG ${GCC_FLAGS} -g3 -O0)
set(GCC_FLAGS_RELEASE ${GCC_FLAGS} -DNDEBUG -O3)