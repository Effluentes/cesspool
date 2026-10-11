#pragma once

#include "domain/WaterLevel.hh"

namespace cesspool::domain
{
struct SystemState
{
    WaterLevel waterLevel{};
    bool bleConnected = false;
};
} // namespace cesspool::domain
