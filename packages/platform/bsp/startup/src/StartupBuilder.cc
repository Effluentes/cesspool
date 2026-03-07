#include "platform/bsp/startup/StartupBuilder.hh"

namespace platform::bsp
{
    StartupResult StartupBuilder::build()
    {
        if(with.nvs)
        {
            if (auto r = initializeNvs(); !r.has_value())
            {
                return std::unexpected(StartupError::NvsInitFailed);
            }
        }
        return {};
    }

    StartupBuilder& StartupBuilder::withNvs()
    {
        with.nvs = true;
        return *this;
    }

    StartupResult StartupBuilder::initializeNvs()
    {
        return {};
    }
}