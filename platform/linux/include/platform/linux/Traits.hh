#pragma once

#include "domain/Errors.hh"
#include "ports/BleTransport.hh"
#include "ports/EncoderSource.hh"
#include "ports/PlatformTraits.hh"

#include <cstdio>
#include <expected>
#include <span>
#include <string_view>

namespace platform::linux
{
class LinuxDisplay
{
public:
    std::expected<void, cesspool::domain::DisplayError> draw(std::string_view text)
    {
        std::printf("display: %.*s\n", static_cast<int>(text.size()), text.data());
        return {};
    }
};

class LinuxBleTransport
{
public:
    std::expected<void, cesspool::domain::BleError> send(std::span<std::uint8_t const>)
    {
        return {};
    }

    void onConnect(ConnectCallback callback, void *context)
    {
        connectCallback_ = callback;
        context_ = context;
    }

    void onData(BleDataCallback callback, void *context)
    {
        dataCallback_ = callback;
        context_ = context;
    }

private:
    ConnectCallback connectCallback_ = nullptr;
    BleDataCallback dataCallback_ = nullptr;
    void *context_ = nullptr;
};

class LinuxEncoderSource
{
public:
    std::expected<EncoderDelta, cesspool::domain::EncoderError> poll()
    {
        return std::unexpected(cesspool::domain::EncoderError::NotReady);
    }
};

struct LinuxTraits
{
    using Display = LinuxDisplay;
    using Ble = LinuxBleTransport;
    using Encoder = LinuxEncoderSource;
};

static_assert(PlatformTraits<LinuxTraits>);
} // namespace platform::linux
