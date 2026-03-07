#pragma once
#include <platform/bsp/startup_result/StartupError.hh>

namespace platform::bsp::interface
{
class Storage
{
public:
    virtual ~Storage() = default;
    virtual StartupResult initialize() = 0;
};
}
