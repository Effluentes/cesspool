#pragma once

namespace cesspool::domain
{
enum class DisplayError
{
    InvalidText,
    RenderFailed,
};

enum class BleError
{
    NotReady,
    SendFailed,
    InvalidPayload,
};

enum class EncoderError
{
    NotReady,
    ReadFailed,
};
} // namespace cesspool::domain
