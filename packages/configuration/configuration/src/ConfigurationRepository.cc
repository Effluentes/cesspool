#include "configuration/configuration/ConfigurationRepository.hh"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace configuration {
    std::expected<ConfigurationRepository::Slot, ConfigurationRepositoryError>
    ConfigurationRepository::readSlot(std::uint8_t index) const noexcept {
        if (storage_.readFunction == nullptr) {
            return std::unexpected(ConfigurationRepositoryError::storageFailure);
        }
        Slot slot;
        slot.index = index;
        const auto readResult = storage_.readFunction(storage_.context, index, slot.record.bytes);
        if (!readResult) {
            if (readResult.error() == ConfigurationStorageError::notFound) {
                return slot;
            }
            return std::unexpected(ConfigurationRepositoryError::storageFailure);
        }

        slot.present = true;
        slot.size = *readResult;
        if (slot.size > slot.record.bytes.size()) {
            slot.error = ConfigurationRecordError::invalidFormat;
            return slot;
        }

        auto decoded = decodeConfigurationRecord(
            std::span<std::byte const>{slot.record.bytes.data(), slot.size});
        if (!decoded) {
            slot.error = decoded.error();
            return slot;
        }
        slot.decoded = std::move(*decoded);
        slot.valid = true;
        return slot;
    }

    bool ConfigurationRepository::isNewer(std::uint32_t left, std::uint32_t right) noexcept {
        return static_cast<std::int32_t>(left - right) > 0;
    }

    ConfigurationRepository::Slot *ConfigurationRepository::newestValid(Slot &left,
                                                                        Slot &right) noexcept {
        if (!left.valid) {
            return right.valid ? &right : nullptr;
        }
        if (!right.valid || !isNewer(right.decoded.generation, left.decoded.generation)) {
            return &left;
        }
        return &right;
    }

    std::expected<Configuration, ConfigurationRepositoryError>
    ConfigurationRepository::load() const noexcept {
        auto slotA = readSlot(0);
        if (!slotA) {
            return std::unexpected(slotA.error());
        }
        auto slotB = readSlot(1);
        if (!slotB) {
            return std::unexpected(slotB.error());
        }

        auto *selected = newestValid(*slotA, *slotB);
        if (selected == nullptr) {
            if (!slotA->present && !slotB->present) {
                return std::unexpected(ConfigurationRepositoryError::notFound);
            }
            if ((slotA->present && slotA->error == ConfigurationRecordError::unsupportedVersion) ||
                (slotB->present && slotB->error == ConfigurationRecordError::unsupportedVersion)) {
                return std::unexpected(ConfigurationRepositoryError::unsupportedRecordVersion);
            }
            return std::unexpected(ConfigurationRepositoryError::invalidRecord);
        }
        return std::move(selected->decoded.configuration);
    }

    std::expected<void, ConfigurationRepositoryError>
    ConfigurationRepository::save(Configuration const &configuration) noexcept {
        if (storage_.readFunction == nullptr || storage_.writeFunction == nullptr) {
            return std::unexpected(ConfigurationRepositoryError::storageFailure);
        }
        auto slotA = readSlot(0);
        if (!slotA) {
            return std::unexpected(slotA.error());
        }
        auto slotB = readSlot(1);
        if (!slotB) {
            return std::unexpected(slotB.error());
        }

        const auto *current = newestValid(*slotA, *slotB);
        const auto nextGeneration = current == nullptr ? 1U : current->decoded.generation + 1U;
        const auto targetSlot =
            current == nullptr ? std::uint8_t{0} : static_cast<std::uint8_t>(current->index ^ 1U);

        const auto encoded = encodeConfigurationRecord(configuration, nextGeneration);
        if (!encoded) {
            return std::unexpected(ConfigurationRepositoryError::encodingFailure);
        }
        const auto record = std::span<std::byte const>{encoded->bytes.data(), encoded->size};

        if (!storage_.writeFunction(storage_.context, targetSlot, record)) {
            return std::unexpected(ConfigurationRepositoryError::storageFailure);
        }

        const auto verified = readSlot(targetSlot);
        if (!verified) {
            return std::unexpected(verified.error());
        }
        if (!verified->valid || verified->decoded.generation != nextGeneration) {
            return std::unexpected(ConfigurationRepositoryError::verificationFailure);
        }
        return {};
    }
} // namespace configuration
