#pragma once

#include "configuration/configuration/Configuration.hh"

#include <array>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <span>

namespace configuration {
    inline constexpr std::uint8_t configurationRecordSchemaVersion = 1;
    inline constexpr std::size_t maximumConfigurationRecordSize = 107;

    enum class ConfigurationRecordError : std::uint8_t {
        unsupportedVersion,
        invalidFormat,
        checksumMismatch
    };

    struct EncodedConfigurationRecord {
        std::array<std::byte, maximumConfigurationRecordSize> bytes{};
        std::size_t size{0};
    };

    struct DecodedConfigurationRecord {
        Configuration configuration;
        std::uint32_t generation{0};
    };

    [[nodiscard]] std::expected<EncodedConfigurationRecord, ConfigurationRecordError>
    encodeConfigurationRecord(Configuration const &configuration,
                              std::uint32_t generation) noexcept;

    [[nodiscard]] std::expected<DecodedConfigurationRecord, ConfigurationRecordError>
    decodeConfigurationRecord(std::span<std::byte const> bytes) noexcept;
} // namespace configuration
