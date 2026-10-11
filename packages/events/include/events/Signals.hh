#pragma once

#include <cstdint>

namespace cesspool::events
{
enum class Signal : std::uint8_t
{
    WaterLevelChanged,
    BleConnected,
    BleDisconnected,
    BleDataReceived,
    EncoderTurned,
};
} // namespace cesspool::events
