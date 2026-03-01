#include "platform/bluetooth/gap/NimBLEGAP.hh"

namespace platform::bluetooth::nimble {

NimBLEGAP::NimBLEGAP(NimBLEServer& server)
    : server_(server)
    , advertising_(NimBLEDevice::getAdvertising())
    , connected_(false)
{
    server_.setCallbacks(this);
}

void NimBLEGAP::startAdvertising() {
    advertising_->start();
}

void NimBLEGAP::stopAdvertising() {
    advertising_->stop();
}

bool NimBLEGAP::isAdvertising() const {
    return NimBLEDevice::getAdvertising()->isAdvertising();
}

bool NimBLEGAP::isConnected() const {
    return connected_;
}

void NimBLEGAP::addObserver(interface::BluetoothGAPObserver& observer) {
    observers_.push_back(&observer);
}

void NimBLEGAP::removeObserver(interface::BluetoothGAPObserver& observer) {
    observers_.erase(
        std::remove(observers_.begin(), observers_.end(), &observer),
        observers_.end()
    );
}

// --- NimBLE callbacks ---

void NimBLEGAP::onConnect(NimBLEServer* server, NimBLEConnInfo& info) {
    connected_ = true;
    notifyObservers({ info.getAddress().toString() }, true);
}

void NimBLEGAP::onDisconnect(NimBLEServer* server, NimBLEConnInfo& info, int reason) {
    connected_ = false;
    notifyObservers({ info.getAddress().toString() }, false);
}

void NimBLEGAP::notifyObservers(const interface::ConnectionEvent& event, bool connected) {
    for (auto* observer : observers_) {
        if (connected) observer->onConnect(event);
        else           observer->onDisconnect(event);
    }
}

} // namespace platform::bluetooth::nimble