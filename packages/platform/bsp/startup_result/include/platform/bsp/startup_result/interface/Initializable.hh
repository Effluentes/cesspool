#pragma once

namespace platform::bsp::interface
{
struct Initializable
{
    virtual ~Initializable() = default;
    virtual void initialize() = 0;
    virtual bool isInitialized() const = 0;
};
}