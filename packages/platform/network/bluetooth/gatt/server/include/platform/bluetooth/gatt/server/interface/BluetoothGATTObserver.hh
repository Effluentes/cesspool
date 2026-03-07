#pragma once

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
}