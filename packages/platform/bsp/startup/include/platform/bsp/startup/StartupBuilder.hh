#pragma once
#include "platform/bsp/startup_result/StartupError.hh"
#include <platform/bsp/storage/storage/interface/Storage.hh>
#include <flat_set>
namespace platform::bsp
{
class StartupBuilder
{
    public:
    StartupBuilder() = default;

    StartupResult initialize();

    StartupBuilder& withStorage(interface::Storage& storage);

    private:
    static constexpr std::size_t MAX_STORAGES = 8U;
    std::flat_set<interface::Storage*> registeredStorages_;
};
}