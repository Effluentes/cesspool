#include "platform/bsp/startup/StartupBuilder.hh"
#include <platform/bsp/storage/nvs/NonVolatileStorage.hh>

namespace platform::bsp
{
    StartupResult StartupBuilder::initialize()
    {
        for (auto* storage : registeredStorages_)
        {
            if (auto r = storage->initialize(); !r.has_value())
            {
                return std::unexpected(StartupError::STORAGE_INIT_FAILED);
            }
        }
        return {};
    }

    StartupBuilder& StartupBuilder::withStorage(interface::Storage& storage)
    {
        if (registeredStorages_.size() >= MAX_STORAGES) {
            // Q_ASSERT(false && "maximum number of storages reached");
            return *this;
        }
        if(registeredStorages_.contains(&storage)) {
            // Q_ASSERT(false && "storage already registered — check main.cc for duplicates");
            return *this;
        }
        registeredStorages_.insert(&storage);
        return *this;
    }
}