#include <configuration/configuration/Configuration.hh>
#include <configuration/configuration/ConfigurationRepository.hh>
#include <configuration/configuration/ConfigurationRecord.hh>

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <utility>

TEST_CASE("fixed text keeps a bounded null-terminated value") {
    configuration::FixedText<5> text;

    REQUIRE(text.capacity() == 5);
    REQUIRE(text.setSize(4));
    REQUIRE(text.size() == 4);
    REQUIRE(text.data()[4] == '\0');
    REQUIRE_FALSE(text.setSize(text.capacity()));
}

TEST_CASE("secret text is movable and clears the moved-from buffer") {
    configuration::SecretText original;
    original.data()[0] = 's';
    original.data()[1] = 'e';
    original.data()[2] = 'c';
    REQUIRE(original.setSize(3));

    auto moved = std::move(original);

    REQUIRE(moved.data()[0] == 's');
    REQUIRE(moved.data()[1] == 'e');
    REQUIRE(moved.data()[2] == 'c');
    REQUIRE(original.data()[0] == '\0');
    REQUIRE(original.data()[1] == '\0');
    REQUIRE(original.data()[2] == '\0');
}

namespace {
    std::uint32_t nextRandom(std::uint32_t &state) {
        state ^= state << 13U;
        state ^= state >> 17U;
        state ^= state << 5U;
        return state;
    }

    configuration::Configuration makeConfiguration(std::uint8_t ssidSize, std::uint8_t passwordSize,
                                                   std::uint32_t &randomState) {
        configuration::Configuration result;
        for (std::size_t index = 0; index < ssidSize; ++index) {
            result.wifi.ssid.data()[index] = static_cast<char>('!' + nextRandom(randomState) % 90U);
        }
        REQUIRE(result.wifi.ssid.setSize(ssidSize));
        for (std::size_t index = 0; index < passwordSize; ++index) {
            result.wifi.password.data()[index] =
                static_cast<char>('!' + nextRandom(randomState) % 90U);
        }
        REQUIRE(result.wifi.password.setSize(passwordSize));
        return result;
    }

    struct MemoryConfigurationSlots {
        std::array<std::array<std::byte, configuration::maximumConfigurationRecordSize>, 2> slots{};
        std::array<std::size_t, 2> sizes{};
        std::array<bool, 2> present{};
        std::size_t failAfterBytes{configuration::maximumConfigurationRecordSize + 1U};

        configuration::ConfigurationStoragePort::ReadResult
        readSlot(std::uint8_t index, std::span<std::byte> destination) noexcept {
            if (!present[index]) {
                return std::unexpected(configuration::ConfigurationStorageError::notFound);
            }
            if (destination.size() < sizes[index]) {
                return std::unexpected(configuration::ConfigurationStorageError::ioFailure);
            }
            std::copy_n(slots[index].begin(), sizes[index], destination.begin());
            return sizes[index];
        }

        configuration::ConfigurationStoragePort::WriteResult
        writeSlot(std::uint8_t index, std::span<std::byte const> record) noexcept {
            const auto bytesToWrite = std::min(record.size(), failAfterBytes);
            std::copy_n(record.begin(), bytesToWrite, slots[index].begin());
            sizes[index] = bytesToWrite;
            present[index] = true;
            if (bytesToWrite != record.size()) {
                return std::unexpected(configuration::ConfigurationStorageError::ioFailure);
            }
            return {};
        }
    };

    bool sameConfiguration(configuration::Configuration const &left,
                           configuration::Configuration const &right) {
        return left.wifi.ssid.size() == right.wifi.ssid.size() &&
               left.wifi.password.size() == right.wifi.password.size() &&
               std::equal(left.wifi.ssid.data(), left.wifi.ssid.data() + left.wifi.ssid.size(),
                          right.wifi.ssid.data()) &&
               std::equal(left.wifi.password.data(),
                          left.wifi.password.data() + left.wifi.password.size(),
                          right.wifi.password.data());
    }
} // namespace

TEST_CASE("configuration record round-trips generated Wi-Fi values") {
    std::uint32_t randomState = 0xC355B001U;
    for (std::size_t iteration = 0; iteration < 1000; ++iteration) {
        const auto ssidSize = static_cast<std::uint8_t>(1U + nextRandom(randomState) % 32U);
        const auto passwordSize = static_cast<std::uint8_t>(nextRandom(randomState) % 65U);
        auto source = makeConfiguration(ssidSize, passwordSize, randomState);

        const auto encoded =
            configuration::encodeConfigurationRecord(source, static_cast<std::uint32_t>(iteration));
        REQUIRE(encoded.has_value());

        const auto decoded = configuration::decodeConfigurationRecord(
            std::span<std::byte const>{encoded->bytes.data(), encoded->size});
        REQUIRE(decoded.has_value());
        REQUIRE(decoded->generation == iteration);
        REQUIRE(decoded->configuration.wifi.ssid.size() == ssidSize);
        REQUIRE(decoded->configuration.wifi.password.size() == passwordSize);
        REQUIRE(std::equal(source.wifi.ssid.data(), source.wifi.ssid.data() + ssidSize,
                           decoded->configuration.wifi.ssid.data()));
        REQUIRE(std::equal(source.wifi.password.data(), source.wifi.password.data() + passwordSize,
                           decoded->configuration.wifi.password.data()));
    }
}

TEST_CASE("configuration record rejects truncation and checksum corruption") {
    std::uint32_t randomState = 0x51A7E123U;
    auto source = makeConfiguration(17, 26, randomState);
    const auto encoded = configuration::encodeConfigurationRecord(source, 9);
    REQUIRE(encoded.has_value());

    for (std::size_t size = 0; size < encoded->size; ++size) {
        REQUIRE_FALSE(configuration::decodeConfigurationRecord(
                          std::span<std::byte const>{encoded->bytes.data(), size})
                          .has_value());
    }

    auto corrupted = encoded->bytes;
    corrupted[encoded->size / 2] ^= std::byte{1};
    const auto result = configuration::decodeConfigurationRecord(
        std::span<std::byte const>{corrupted.data(), encoded->size});
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == configuration::ConfigurationRecordError::checksumMismatch);
}

TEST_CASE("dual-slot save recovers the last valid record after every partial write") {
    std::uint32_t randomState = 0x712EAC09U;
    const auto first = makeConfiguration(10, 12, randomState);
    const auto second = makeConfiguration(15, 20, randomState);
    const auto replacement = makeConfiguration(21, 32, randomState);
    const auto replacementRecord = configuration::encodeConfigurationRecord(replacement, 3);
    REQUIRE(replacementRecord.has_value());

    MemoryConfigurationSlots baseline;
    configuration::ConfigurationRepository baselineRepository{
        configuration::ConfigurationStoragePort::bind(baseline)};
    REQUIRE(baselineRepository.save(first));
    REQUIRE(baselineRepository.save(second));

    for (std::size_t cut = 0; cut < replacementRecord->size; ++cut) {
        auto interruptedStorage = baseline;
        interruptedStorage.failAfterBytes = cut;
        configuration::ConfigurationRepository interruptedRepository{
            configuration::ConfigurationStoragePort::bind(interruptedStorage)};

        const auto interruptedSave = interruptedRepository.save(replacement);
        REQUIRE_FALSE(interruptedSave.has_value());

        interruptedStorage.failAfterBytes = configuration::maximumConfigurationRecordSize + 1U;
        auto recovered = interruptedRepository.load();
        REQUIRE(recovered.has_value());
        REQUIRE(sameConfiguration(*recovered, second));
    }

    auto successfulStorage = baseline;
    configuration::ConfigurationRepository successfulRepository{
        configuration::ConfigurationStoragePort::bind(successfulStorage)};
    REQUIRE(successfulRepository.save(replacement));
    const auto recovered = successfulRepository.load();
    REQUIRE(recovered.has_value());
    REQUIRE(sameConfiguration(*recovered, replacement));
}
