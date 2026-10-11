#pragma once

#include "domain/Errors.hh"

#include <concepts>
#include <cstdint>
#include <expected>
#include <span>

using ConnectCallback = void (*)(void *context, bool connected);
using BleDataCallback = void (*)(void *context, std::span<std::uint8_t const> payload);

template <typename T>
concept BleTransport = requires(
    T &transport,
    std::span<std::uint8_t const> payload,
    ConnectCallback connectCallback,
    BleDataCallback dataCallback,
    void *context)
{
    { transport.send(payload) } -> std::same_as<std::expected<void, cesspool::domain::BleError>>;
    { transport.onConnect(connectCallback, context) } -> std::same_as<void>;
    { transport.onData(dataCallback, context) } -> std::same_as<void>;
};
