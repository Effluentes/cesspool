#pragma once

#include "platform/bluetooth/gatt/interface/BluetoothGATTServer.hh"
#include <NimBLEDevice.h>
#include <vector>

namespace platform::bluetooth::nimble {

class NimBLEGATTServer final
    : public interface::BluetoothGATTServer
    , public NimBLECharacteristicCallbacks
{
public:
    explicit NimBLEGATTServer(NimBLEServer& server);
    ~NimBLEGATTServer() override = default;

    void init(const std::string& serviceUUID,
              const std::string& characteristicUUID) override;

    void addObserver(interface::BluetoothGATTObserver& observer) override;
    void removeObserver(interface::BluetoothGATTObserver& observer) override;

private:
    void onWrite(NimBLECharacteristic* characteristic,
                 NimBLEConnInfo& info) override;

    void notifyObservers(const interface::WriteEvent& event);

    NimBLEServer&                                   server_;
    NimBLEService*                                  service_;
    NimBLECharacteristic*                           characteristic_;
    std::vector<interface::BluetoothGATTObserver*>  observers_;
};

} // namespace platform::bluetooth::nimble