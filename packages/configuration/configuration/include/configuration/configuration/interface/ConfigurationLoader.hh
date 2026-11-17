#pragma once
#include "configuration/configuration/Configuration.hh"
#include <concepts>
#include <expected>

namespace configuration::interface
{
    enum class ConfigurationLoaderError
    {
        CONFIGURATION_NOT_FOUND,
        INVALID_FORMAT,
        UNKNOWN_ERROR
    };

    using ConfigurationLoadResult =
        std::expected<configuration::Configuration, ConfigurationLoaderError>;

    template <typename T>
    concept ConfigurationLoaderPort = requires(T &loader)
    {
        { loader.loadConfiguration() } -> std::same_as<ConfigurationLoadResult>;
    };

    struct ConfigurationLoader
    {
        void *context;
        ConfigurationLoadResult (*loadFunction)(void *);

        template <ConfigurationLoaderPort Adapter>
        [[nodiscard]] static ConfigurationLoader bind(Adapter &adapter) noexcept
        {
            return {
                &adapter,
                [](void *context)
                {
                    return static_cast<Adapter *>(context)->loadConfiguration();
                }};
        }

        [[nodiscard]] ConfigurationLoadResult loadConfiguration() const
        {
            return loadFunction(context);
        }
    };
}
