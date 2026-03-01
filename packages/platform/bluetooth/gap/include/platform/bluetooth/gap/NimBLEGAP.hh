#pragma once

#include "platform/bluetooth/gap/interface/GAP.hh"
#include <NimBLEDevice.h>
#include <vector>

namespace platform::bluetooth {

class NimBLEGAP final
    : public interface::GAP
    , public NimBLEServerCallbacks
{
public:
    explicit NimBLEGAP(NimBLEServer& );
    ~NimBLEGAP() override = default;

    void startAdvertising() override;
    void stopAdvertising()  override;
    bool isAdvertising() const override;
    bool isConnected()   const override;

    void addObserver(interface::BluetoothGAPObserver& ) override;
    void removeObserver(interface::BluetoothGAPObserver& ) override;

private:
    void onConnect(NimBLEServer* , NimBLEConnInfo& ) override;
    void onDisconnect(NimBLEServer* , NimBLEConnInfo& , int ) override;

    void notifyObservers(const interface::ConnectionEvent& , bool );

    NimBLEServer&                                  server_;
    NimBLEAdvertising*                             advertising_;
    std::vector<interface::BluetoothGAPObserver*>  observers_;
    bool                                           connected_;
};

} // namespace platform::bluetooth::nimble