#pragma once
#include <expected>

namespace platform::bsp
{
enum class StartupError
{
    //storage
    STORAGE_INIT_FAILED = 0,
    //NVS
    NVS_NO_FREE_PAGES = 10,
    NVS_NOT_FOUND_NVS_PARTITION,
    NVS_NO_MEM,
    NVS_ENCRYPTION_FAILED = 19
};
using StartupResult = std::expected<void, StartupError>;
}