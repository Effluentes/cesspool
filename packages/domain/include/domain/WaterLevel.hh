#pragma once

#include <cstdint>

namespace cesspool::domain
{
class WaterLevel
{
public:
    constexpr explicit WaterLevel(std::uint16_t millimetres = 0) noexcept
        : millimetres_(millimetres)
    {}

    constexpr std::uint16_t millimetres() const noexcept
    {
        return millimetres_;
    }

    friend constexpr bool operator==(WaterLevel, WaterLevel) = default;

private:
    std::uint16_t millimetres_;
};
} // namespace cesspool::domain
