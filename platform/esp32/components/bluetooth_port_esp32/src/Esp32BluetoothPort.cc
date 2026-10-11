#include "platform_bluetooth_esp32/Esp32BluetoothPort.hh"

#include <algorithm>
#include <cstring>

#include "host/ble_gap.h"
#include "nimble/nimble_port.h"
#include "os/os_mbuf.h"
#include "services/gap/ble_svc_gap.h"

std::expected<void, cesspool::domain::BleError> Esp32BluetoothPort::send(
    std::span<std::uint8_t const> payload)
{
    if (payload.empty()) {
        return std::unexpected(cesspool::domain::BleError::InvalidPayload);
    }
    return {};
}

void Esp32BluetoothPort::onConnect(ConnectCallback callback, void *context)
{
    connectCallback_ = callback;
    context_ = context;
    startAdvertising();
}

void Esp32BluetoothPort::onData(BleDataCallback callback, void *context)
{
    dataCallback_ = callback;
    context_ = context;
}

void Esp32BluetoothPort::startAdvertising()
{
    ble_svc_gap_device_name_set("ESP32-Provision");

    ble_hs_adv_fields fields{};
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    auto const *name = reinterpret_cast<std::uint8_t const *>("ESP32_NIMBLE");
    fields.name = const_cast<std::uint8_t *>(name);
    fields.name_len = std::strlen("ESP32_NIMBLE");
    fields.name_is_complete = 1;
    if (ble_gap_adv_set_fields(&fields) != 0) {
        return;
    }

    ble_gap_adv_params parameters{};
    parameters.conn_mode = BLE_GAP_CONN_MODE_UND;
    parameters.disc_mode = BLE_GAP_DISC_MODE_GEN;
    (void)ble_gap_adv_start(
        BLE_OWN_ADDR_PUBLIC,
        nullptr,
        BLE_HS_FOREVER,
        &parameters,
        &Esp32BluetoothPort::gap_event_callback,
        this);
}

int Esp32BluetoothPort::gap_event_callback(ble_gap_event *event, void *context)
{
    static_cast<Esp32BluetoothPort *>(context)->handle_gap_event(event);
    return 0;
}

void Esp32BluetoothPort::handle_gap_event(ble_gap_event *event)
{
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        if (event->connect.status == 0) {
            connectionHandle_ = event->connect.conn_handle;
            if (connectCallback_ != nullptr) {
                connectCallback_(context_, true);
            }
        }
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        connectionHandle_ = 0;
        if (connectCallback_ != nullptr) {
            connectCallback_(context_, false);
        }
        break;

    case BLE_GAP_EVENT_NOTIFY_RX: {
        auto const *message = event->notify_rx.om;
        auto const length = std::min<std::uint16_t>(OS_MBUF_PKTLEN(message), 127U);
        std::uint8_t data[128]{};
        os_mbuf_copydata(message, 0, length, data);
        if (dataCallback_ != nullptr) {
            dataCallback_(context_, std::span<std::uint8_t const>{data, length});
        }
        break;
    }

    case BLE_GAP_EVENT_ADV_COMPLETE:
        startAdvertising();
        break;

    default:
        break;
    }
}
