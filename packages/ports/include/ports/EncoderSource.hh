#pragma once

#include "domain/Errors.hh"

#include <concepts>
#include <cstdint>
#include <expected>

struct EncoderDelta
{
    std::int8_t value = 0;
};

template <typename T>
concept EncoderSource = requires(T &encoder)
{
    { encoder.poll() } -> std::same_as<std::expected<EncoderDelta, cesspool::domain::EncoderError>>;
};
