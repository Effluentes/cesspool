#pragma once
#include <functional>
#include "platform/bluetooth/gap/interface/GAPObserver.hh"

namespace platform::bluetooth::interface
{
class GAP {
public:
    virtual ~IBluetoothGAP() = default;

    virtual void startAdvertising() = 0;
    virtual void stopAdvertising()  = 0;
    virtual bool isAdvertising() const = 0;
    virtual bool isConnected()  const = 0;

    virtual void addObserver(GAPObserver& )    = 0;
    virtual void removeObserver(GAPObserver& ) = 0;
};
}