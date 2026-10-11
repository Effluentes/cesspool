#pragma once

#include "events/Signals.hh"
#include "domain/WaterLevel.hh"

#include <array>
#include <cstdint>

namespace cesspool::events
{
struct WaterLevelChangedEvt
{
    Signal signal = Signal::WaterLevelChanged;
    domain::WaterLevel level{};
};

struct BleConnectedEvt
{
    Signal signal = Signal::BleConnected;
    bool connected = false;
};

struct EncoderTurnedEvt
{
    Signal signal = Signal::EncoderTurned;
    std::int8_t delta = 0;
};
} // namespace cesspool::events
