#pragma once

namespace communication::radio::transceivers::interface
{
class BluetoothHCI
{
public:
    virtual ~BluetoothHCI() = default;
    virtual bool initialize() = 0;
};
}