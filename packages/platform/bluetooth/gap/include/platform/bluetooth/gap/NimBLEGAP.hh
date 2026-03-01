#pragma once

#include "platform/bluetooth/gap/interface/BluetoothGAP.hh"
#include <NimBLEDevice.h>
#include <vector>

namespace platform::bluetooth::nimble {

class NimBLEGAP final
    : public interface::BluetoothGAP
    , public NimBLEServerCallbacks
{
public:
    explicit NimBLEGAP(NimBLEServer& server);
    ~NimBLEGAP() override = default;

    // BluetoothGAP
    void startAdvertising() override;
    void stopAdvertising()  override;
    bool isAdvertising() const override;
    bool isConnected()   const override;

    void addObserver(interface::BluetoothGAPObserver& observer) override;
    void removeObserver(interface::BluetoothGAPObserver& observer) override;

private:
    void onConnect(NimBLEServer* server, NimBLEConnInfo& info) override;
    void onDisconnect(NimBLEServer* server, NimBLEConnInfo& info, int reason) override;

    void notifyObservers(const interface::ConnectionEvent& event, bool connected);

    NimBLEServer&                                  server_;
    NimBLEAdvertising*                             advertising_;
    std::vector<interface::BluetoothGAPObserver*>  observers_;
    bool                                           connected_;
};

} // namespace platform::bluetooth::nimble