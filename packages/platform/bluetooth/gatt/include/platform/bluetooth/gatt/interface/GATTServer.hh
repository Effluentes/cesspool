// platform/bluetooth/gatt/server/include/interface/GATTServer.hh

#pragma once

#include <functional>
#include <string>

namespace platform::bluetooth::interface {

struct WriteEvent {
    std::string macAddress;
    std::string data;
};

class GATTObserver {
public:
    virtual ~GATTObserver() = default;
    virtual void onWrite(const WriteEvent& event) = 0;
};

class GATTServer {
public:
    virtual ~GATTServer() = default;

    virtual void init(const std::string& serviceUUID,
                      const std::string& characteristicUUID) = 0;

    virtual void addObserver(GATTObserver& observer)    = 0;
    virtual void removeObserver(GATTObserver& observer) = 0;
};

} // namespace platform::bluetooth::interface