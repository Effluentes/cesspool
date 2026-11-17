#include <configuration/configuration/ConfigurationRecord.hh>

#include <cstddef>
#include <cstdint>
#include <span>

extern "C" int LLVMFuzzerTestOneInput(std::uint8_t const *data, std::size_t size) {
    const auto bytes = std::span<std::byte const>{reinterpret_cast<std::byte const *>(data), size};
    const auto decoded = configuration::decodeConfigurationRecord(bytes);
    if (!decoded) {
        return 0;
    }

    const auto encoded =
        configuration::encodeConfigurationRecord(decoded->configuration, decoded->generation);
    if (!encoded) {
        __builtin_trap();
    }

    const auto roundTrip = configuration::decodeConfigurationRecord(
        std::span<std::byte const>{encoded->bytes.data(), encoded->size});
    if (!roundTrip || roundTrip->generation != decoded->generation) {
        __builtin_trap();
    }
    return 0;
}
