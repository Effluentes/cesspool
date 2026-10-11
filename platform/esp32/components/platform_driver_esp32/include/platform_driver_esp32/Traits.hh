#pragma once

#include "platform_driver_esp32/PlatformAdapters.hh"
#include <platform_bluetooth_esp32/Esp32BluetoothPort.hh>

namespace platform::esp32
{
struct Esp32Traits
{
    using Display = ::Esp32Display;
    using Ble = ::Esp32BluetoothPort;
    using Encoder = ::Esp32EncoderSource;
};

static_assert(PlatformTraits<Esp32Traits>);
} // namespace platform::esp32
