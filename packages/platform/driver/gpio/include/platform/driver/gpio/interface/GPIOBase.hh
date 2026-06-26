#pragma once

namespace platform::driver::interface
{
template <typename Derived>
struct GPIOBase {
    void setPinHigh() { static_cast<Derived*>(this)->setPinHighImpl(); }
    void setPinLow()  { static_cast<Derived*>(this)->setPinLowImpl();  }
    bool readPin()const    { return static_cast<const Derived*>(this)->readPinImpl(); }
protected:
    ~GPIOBase() = default;
};
}