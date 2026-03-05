#pragma once
#include <functional>
#include <string>

namespace platform::bluetooth::interface
{
struct ConnectionEvent {
    std::string macAddress;
};

class GAPObserver {
public:
    virtual ~GAPObserver() = default;
    virtual void onConnect(const ConnectionEvent& )    = 0;
    virtual void onDisconnect(const ConnectionEvent& ) = 0;
};

}