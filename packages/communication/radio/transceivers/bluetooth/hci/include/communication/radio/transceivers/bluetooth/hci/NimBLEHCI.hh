#pragma once

#include "communication/radio/transceivers/bluetooth/hci/interface/BluetoothHCI.hh"

namespace communication::radio::transceivers
{
class NimBLEHCI : public interface::BluetoothHCI
{
public:
    NimBLEHCI();
    ~NimBLEHCI() override;
    bool initialize() override;
};
}
