#pragma once

#include <ports/BleTransport.hh>

#include <cstdint>
#include <expected>

struct ble_gap_event;

class Esp32BluetoothPort final
{
public:
    std::expected<void, cesspool::domain::BleError> send(
        std::span<std::uint8_t const> payload);
    void onConnect(ConnectCallback callback, void *context);
    void onData(BleDataCallback callback, void *context);

private:
    static int gap_event_callback(ble_gap_event *event, void *context);
    void handle_gap_event(ble_gap_event *event);
    void startAdvertising();

    ConnectCallback connectCallback_ = nullptr;
    BleDataCallback dataCallback_ = nullptr;
    void *context_ = nullptr;
    uint16_t connectionHandle_ = 0;
};
