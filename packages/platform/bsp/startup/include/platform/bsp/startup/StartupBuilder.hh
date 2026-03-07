#pragma once
#include <expected>

namespace platform::bsp
{
class StartupBuilder
{
    public:
    enum class StartupError
    {
        NvsInitFailed = 0
    };
    using Result = std::expected<void, StartupError>;
    StartupBuilder() = default;

    Result build();

    StartupBuilder& withNvs();

    private:
    struct
    {
        bool nvs = false;
    }with;

    Result initializeNvs();
};
}