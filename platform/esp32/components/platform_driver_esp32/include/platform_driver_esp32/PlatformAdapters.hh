#pragma once

#include "platform_driver_esp32/EncoderDriver.hh"
#include "platform_driver_esp32/GPIO.hh"
#include "platform_driver_esp32/I2C.hh"
#include "platform_driver_esp32/Sh1106.hh"
#include "ports/EncoderSource.hh"

#include <expected>
#include <string_view>

class Esp32Display
{
public:
    Esp32Display()
        : sda_(GPIO_NUM_6)
        , scl_(GPIO_NUM_7)
        , wires_(sda_, scl_)
        , i2c_(wires_)
        , driver_(i2c_)
    {}

    std::expected<void, cesspool::domain::DisplayError> draw(std::string_view text)
    {
        (void)text;
        driver_.clear();
        return {};
    }

private:
    Esp32GPIODriver sda_;
    Esp32GPIODriver scl_;
    Esp32I2CWires wires_;
    Esp32I2CDriver i2c_;
    Sh1106Driver driver_;
};

class Esp32EncoderSource
{
public:
    Esp32EncoderSource()
        : driver_(GPIO_NUM_8, GPIO_NUM_9, GPIO_NUM_18)
    {}

    std::expected<EncoderDelta, cesspool::domain::EncoderError> poll()
    {
        return std::unexpected(cesspool::domain::EncoderError::NotReady);
    }

private:
    EncoderDriver driver_;
};
