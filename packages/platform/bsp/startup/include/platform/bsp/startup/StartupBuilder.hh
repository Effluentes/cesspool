#pragma once
#include "platform/bsp/startup_result/StartupError.hh"
namespace platform::bsp
{
class StartupBuilder
{
    public:
    StartupBuilder() = default;

    StartupResult build();

    StartupBuilder& withNvs();

    private:
    struct
    {
        bool nvs = false;
    }with;

    StartupResult initializeNvs();
};
}