#pragma once
#include "platform/bsp/storage/storage/interface/Storage.hh"
#include "platform/bsp/startup_result/StartupError.hh"

namespace platform::bsp
{
class NonVolatileStorage : public interface::Storage
{
public:
    NonVolatileStorage();
    ~NonVolatileStorage() override;
    StartupResult initialize() override;
private:
    bool erase();
};
}