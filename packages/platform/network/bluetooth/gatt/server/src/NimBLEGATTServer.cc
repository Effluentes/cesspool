#include "platform/bluetooth/gatt/NimBLEGATTServer.hh"

namespace platform::bluetooth {

NimBLEGATTServer::NimBLEGATTServer(NimBLEServer& server)
    : server_(server)
    , service_(nullptr)
    , characteristic_(nullptr)
{}

void NimBLEGATTServer::init(const std::string& serviceUUID,
                             const std::string& characteristicUUID)
{
    service_ = server_.createService(serviceUUID);

    characteristic_ = service_->createCharacteristic(
        characteristicUUID, 
        NIMBLE_PROPERTY::WRITE
        | NIMBLE_PROPERTY::WRITE_NR
    );

    characteristic_->setCallbacks(this);
    service_->start();
}

void NimBLEGATTServer::addObserver(interface::BluetoothGATTObserver& observer) {
    observers_.push_back(&observer);
}

void NimBLEGATTServer::removeObserver(interface::BluetoothGATTObserver& observer) {
    observers_.erase(
        std::remove(observers_.begin(), observers_.end(), &observer),
        observers_.end()
    );
}

// --- NimBLE callback ---

void NimBLEGATTServer::onWrite(NimBLECharacteristic* characteristic,
                                NimBLEConnInfo& info)
{
    interface::WriteEvent event {
        .macAddress = info.getAddress().toString(),
        .data       = characteristic->getValue()
    };

    notifyObservers(event);
}

void NimBLEGATTServer::notifyObservers(const interface::WriteEvent& event) {
    for (auto* observer : observers_) {
        observer->onWrite(event);
    }
}

} // namespace platform::bluetooth::nimble