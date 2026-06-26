#pragma once

namespace platform::driver::interface
{
struct GPIO
{
    virtual ~GPIO() = default;
    virtual void setPinHigh() = 0;
    virtual void setPinLow() = 0;
    virtual bool readPin() const = 0;
};
}