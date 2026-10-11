#pragma once

#include "ports/BleTransport.hh"
#include "ports/Display.hh"
#include "ports/EncoderSource.hh"

#include <concepts>

template <typename T>
concept PlatformTraits =
    Display<typename T::Display> &&
    BleTransport<typename T::Ble> &&
    EncoderSource<typename T::Encoder>;
