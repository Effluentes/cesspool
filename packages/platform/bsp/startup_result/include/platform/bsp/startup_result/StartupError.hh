#pragma once
#include <expected>

namespace platform::bsp
{
enum class StartupError
{
    NvsInitFailed = 0
};
using StartupResult = std::expected<void, StartupError>;
}