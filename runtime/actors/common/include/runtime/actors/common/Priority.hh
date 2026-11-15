#pragma once

#include <cstdint>
#include <limits>

namespace runtime::actors
{
    using Priority = std::uint_fast8_t;
    enum class ActiveObjectPriority : Priority
    {
        RESERVE_PRIORITY = 0,
        SYSTEM_AO_PRIORITY,
        NETWORK_AO_PRIORITY,
        DISPLAY_AO_PRIORITY
    };
}
