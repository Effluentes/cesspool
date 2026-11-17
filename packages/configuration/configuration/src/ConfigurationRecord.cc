#include "configuration/configuration/ConfigurationRecord.hh"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace configuration {
    namespace {
        constexpr std::size_t headerSize = 7;
        constexpr std::size_t checksumSize = 4;

        [[nodiscard]] std::uint32_t checksum(std::span<std::byte const> bytes) noexcept {
            std::uint32_t value = 0xFFFFFFFFU;
            for (const auto byte : bytes) {
                value ^= std::to_integer<std::uint8_t>(byte);
                for (std::uint8_t bit = 0; bit < 8; ++bit) {
                    const auto mask = 0U - (value & 1U);
                    value = (value >> 1U) ^ (0xEDB88320U & mask);
                }
            }
            return ~value;
        }

        void writeU32(std::array<std::byte, maximumConfigurationRecordSize> &bytes,
                      std::size_t offset, std::uint32_t value) noexcept {
            for (std::size_t index = 0; index < sizeof(value); ++index) {
                bytes[offset + index] = static_cast<std::byte>((value >> (index * 8U)) & 0xFFU);
            }
        }

        [[nodiscard]] std::uint32_t readU32(std::span<std::byte const> bytes,
                                            std::size_t offset) noexcept {
            std::uint32_t value = 0;
            for (std::size_t index = 0; index < sizeof(value); ++index) {
                value |=
                    static_cast<std::uint32_t>(std::to_integer<std::uint8_t>(bytes[offset + index]))
                    << (index * 8U);
            }
            return value;
        }
    } // namespace

    std::expected<EncodedConfigurationRecord, ConfigurationRecordError>
    encodeConfigurationRecord(Configuration const &configuration,
                              std::uint32_t generation) noexcept {
        const auto ssidSize = configuration.wifi.ssid.size();
        const auto passwordSize = configuration.wifi.password.size();
        if (ssidSize == 0 || ssidSize >= configuration.wifi.ssid.capacity() ||
            passwordSize >= configuration.wifi.password.capacity()) {
            return std::unexpected(ConfigurationRecordError::invalidFormat);
        }

        EncodedConfigurationRecord encoded;
        encoded.bytes[0] = static_cast<std::byte>(configurationRecordSchemaVersion);
        writeU32(encoded.bytes, 1, generation);
        encoded.bytes[5] = static_cast<std::byte>(ssidSize);
        encoded.bytes[6] = static_cast<std::byte>(passwordSize);
        std::size_t offset = headerSize;
        for (std::size_t index = 0; index < ssidSize; ++index) {
            encoded.bytes[offset++] = static_cast<std::byte>(
                static_cast<unsigned char>(configuration.wifi.ssid.data()[index]));
        }
        for (std::size_t index = 0; index < passwordSize; ++index) {
            encoded.bytes[offset++] = static_cast<std::byte>(
                static_cast<unsigned char>(configuration.wifi.password.data()[index]));
        }

        const auto crc = checksum(std::span<std::byte const>{encoded.bytes.data(), offset});
        writeU32(encoded.bytes, offset, crc);
        encoded.size = offset + checksumSize;
        return encoded;
    }

    std::expected<DecodedConfigurationRecord, ConfigurationRecordError>
    decodeConfigurationRecord(std::span<std::byte const> bytes) noexcept {
        if (bytes.size() < headerSize + checksumSize) {
            return std::unexpected(ConfigurationRecordError::invalidFormat);
        }
        if (std::to_integer<std::uint8_t>(bytes[0]) != configurationRecordSchemaVersion) {
            return std::unexpected(ConfigurationRecordError::unsupportedVersion);
        }

        const auto ssidSize = std::to_integer<std::uint8_t>(bytes[5]);
        const auto passwordSize = std::to_integer<std::uint8_t>(bytes[6]);
        const auto expectedSize = headerSize + static_cast<std::size_t>(ssidSize) +
                                  static_cast<std::size_t>(passwordSize) + checksumSize;
        if (ssidSize == 0 || ssidSize > 32 || passwordSize > 64 || bytes.size() != expectedSize) {
            return std::unexpected(ConfigurationRecordError::invalidFormat);
        }

        const auto checksumOffset = expectedSize - checksumSize;
        if (readU32(bytes, checksumOffset) != checksum(bytes.first(checksumOffset))) {
            return std::unexpected(ConfigurationRecordError::checksumMismatch);
        }

        DecodedConfigurationRecord decoded;
        decoded.generation = readU32(bytes, 1);
        std::size_t offset = headerSize;
        for (std::size_t index = 0; index < ssidSize; ++index) {
            const auto value = std::to_integer<std::uint8_t>(bytes[offset++]);
            if (value == 0) {
                return std::unexpected(ConfigurationRecordError::invalidFormat);
            }
            decoded.configuration.wifi.ssid.data()[index] = static_cast<char>(value);
        }
        for (std::size_t index = 0; index < passwordSize; ++index) {
            const auto value = std::to_integer<std::uint8_t>(bytes[offset++]);
            if (value == 0) {
                return std::unexpected(ConfigurationRecordError::invalidFormat);
            }
            decoded.configuration.wifi.password.data()[index] = static_cast<char>(value);
        }
        if (!decoded.configuration.wifi.ssid.setSize(ssidSize) ||
            !decoded.configuration.wifi.password.setSize(passwordSize)) {
            return std::unexpected(ConfigurationRecordError::invalidFormat);
        }
        return decoded;
    }
} // namespace configuration
