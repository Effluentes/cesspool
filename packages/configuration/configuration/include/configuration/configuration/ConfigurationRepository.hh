#pragma once

#include "configuration/configuration/ConfigurationRecord.hh"

#include <array>
#include <cstddef>
#include <expected>
#include <span>

namespace configuration {
    enum class ConfigurationStorageError { notFound, ioFailure };

    struct ConfigurationStoragePort {
        using ReadResult = std::expected<std::size_t, ConfigurationStorageError>;
        using WriteResult = std::expected<void, ConfigurationStorageError>;

        void *context{nullptr};
        ReadResult (*readFunction)(void *, std::uint8_t, std::span<std::byte>){nullptr};
        WriteResult (*writeFunction)(void *, std::uint8_t, std::span<std::byte const>){nullptr};

        template <typename Adapter>
        [[nodiscard]] static ConfigurationStoragePort bind(Adapter &adapter) noexcept {
            return {&adapter,
                    [](void *context, std::uint8_t slot, std::span<std::byte> destination) {
                        return static_cast<Adapter *>(context)->readSlot(slot, destination);
                    },
                    [](void *context, std::uint8_t slot, std::span<std::byte const> record) {
                        return static_cast<Adapter *>(context)->writeSlot(slot, record);
                    }};
        }
    };

    enum class ConfigurationRepositoryError {
        notFound,
        storageFailure,
        invalidRecord,
        unsupportedRecordVersion,
        encodingFailure,
        verificationFailure
    };

    class ConfigurationRepository final {
      public:
        explicit ConfigurationRepository(ConfigurationStoragePort storage) noexcept
            : storage_(storage) {}

        [[nodiscard]] std::expected<Configuration, ConfigurationRepositoryError>
        load() const noexcept;

        [[nodiscard]] std::expected<void, ConfigurationRepositoryError>
        save(Configuration const &configuration) noexcept;

      private:
        struct Slot {
            EncodedConfigurationRecord record{};
            DecodedConfigurationRecord decoded{};
            std::size_t size{0};
            std::uint8_t index{0};
            bool present{false};
            bool valid{false};
            ConfigurationRecordError error{ConfigurationRecordError::invalidFormat};
        };

        [[nodiscard]] std::expected<Slot, ConfigurationRepositoryError>
        readSlot(std::uint8_t index) const noexcept;
        [[nodiscard]] static bool isNewer(std::uint32_t left, std::uint32_t right) noexcept;
        [[nodiscard]] static Slot *newestValid(Slot &left, Slot &right) noexcept;

        ConfigurationStoragePort storage_;
    };
} // namespace configuration
