#pragma once

namespace runtime::ports
{
class BluetoothPort
{
public:
    enum class Event
    {
        Connected,
        Disconnected,
        Data,
    };

    using EventHandler = void (*)(void *context, Event event, char const *data);

    virtual ~BluetoothPort() = default;
    virtual void start(EventHandler handler, void *context) = 0;
    virtual void stop() = 0;
};
} // namespace runtime::ports
