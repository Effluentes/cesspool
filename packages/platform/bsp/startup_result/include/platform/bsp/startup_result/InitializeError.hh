#pragma once
#include <expected>

namespace platform::bsp
{
enum class InitializeError
{
    NOK = 0
};
using InitializeResult = std::expected<void, InitializeError>;
}