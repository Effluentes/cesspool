#pragma once

class IRadioTransport {
public:
    virtual ~IRadioTransport() = default;

    virtual void init() = 0;
    virtual void startReceive() = 0;
    virtual void send(int slaveId, const void* data, int size) = 0;
};
