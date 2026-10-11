#pragma once

#include "domain/Errors.hh"
#include "ports/BleTransport.hh"
#include "ports/EncoderSource.hh"
#include "ports/PlatformTraits.hh"

#include <expected>
#include <span>
#include <string_view>

namespace platform::stm
{
class StmDisplay
{
public:
    std::expected<void, cesspool::domain::DisplayError> draw(std::string_view)
    {
        return std::unexpected(cesspool::domain::DisplayError::RenderFailed);
    }
};

class StmBleTransport
{
public:
    std::expected<void, cesspool::domain::BleError> send(std::span<std::uint8_t const>)
    {
        return std::unexpected(cesspool::domain::BleError::NotReady);
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

class StmEncoderSource
{
public:
    std::expected<EncoderDelta, cesspool::domain::EncoderError> poll()
    {
        return std::unexpected(cesspool::domain::EncoderError::NotReady);
    }
};

struct StmTraits
{
    using Display = StmDisplay;
    using Ble = StmBleTransport;
    using Encoder = StmEncoderSource;
};

static_assert(PlatformTraits<StmTraits>);
} // namespace platform::stm
