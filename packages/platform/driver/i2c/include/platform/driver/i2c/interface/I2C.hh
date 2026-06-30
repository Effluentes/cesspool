#pragma once
#include <platform/driver/gpio/interface/GPIO.hh>
#include <platform/bsp/startup_result/InitializeError.hh>
#include <cstdint>
namespace platform::driver
{
using I2CPortNumber = std::uint8_t;
using DeviceAddress = std::size_t;
using SCLSpeed = std::size_t;
namespace interface
{
struct I2CConfiguration
{
    const I2CPortNumber portNumber;
    const GPIO& sda;
    const GPIO& scl;
    const DeviceAddress deviceAddress;
    const SCLSpeed sclSpeed;
};
struct I2C
{
    ~I2C() = default;

    virtual platform::bsp::InitializeError initialize() = 0;
};
}
}