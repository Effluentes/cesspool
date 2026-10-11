#include "bluetooth_manager_ao/BluetoothManagerAO.hh"

#include <algorithm>
#include <cstring>

namespace
{
void ledsOff(
    platform::driver::interface::GPIO &red,
    platform::driver::interface::GPIO &blue,
    platform::driver::interface::GPIO &green)
{
    red.setPinLow();
    blue.setPinLow();
    green.setPinLow();
}
} // namespace

BluetoothManagerAO::BluetoothManagerAO(
    platform::driver::interface::GPIO &ledRed,
    platform::driver::interface::GPIO &ledBlue,
    platform::driver::interface::GPIO &ledGreen,
    runtime::ports::BluetoothPort &bluetooth)
    : QActive(Q_STATE_CAST(&initial))
    , m_ledRed(ledRed)
    , m_ledBlue(ledBlue)
    , m_ledGreen(ledGreen)
    , m_bluetooth(bluetooth)
{}

Q_STATE_DEF(BluetoothManagerAO, initial)
{
    (void)e;
    subscribe(BT_START_SIG);
    subscribe(BT_STOP_SIG);
    subscribe(BT_CONNECTED_SIG);
    subscribe(BT_DISCONNECTED_SIG);
    subscribe(BT_DATA_SIG);
    return tran(&disabled);
}

Q_STATE_DEF(BluetoothManagerAO, disabled)
{
    switch (e->sig) {
    case Q_ENTRY_SIG:
        ledsOff(m_ledRed, m_ledBlue, m_ledGreen);
        m_ledRed.setPinHigh();
        return Q_HANDLED();

    case Q_EXIT_SIG:
        m_ledRed.setPinLow();
        return Q_HANDLED();

    case BT_START_SIG:
        return tran(&enabled);
    }
    return super(&QHsm::top);
}

Q_STATE_DEF(BluetoothManagerAO, enabled)
{
    switch (e->sig) {
    case Q_INIT_SIG:
        return tran(&advertising);

    case Q_ENTRY_SIG:
        return Q_HANDLED();

    case Q_EXIT_SIG:
        m_bluetooth.stop();
        ledsOff(m_ledRed, m_ledBlue, m_ledGreen);
        return Q_HANDLED();

    case BT_STOP_SIG:
        return tran(&disabled);
    }
    return super(&QHsm::top);
}

Q_STATE_DEF(BluetoothManagerAO, advertising)
{
    switch (e->sig) {
    case Q_ENTRY_SIG:
        ledsOff(m_ledRed, m_ledBlue, m_ledGreen);
        m_ledBlue.setPinHigh();
        m_bluetooth.start(&BluetoothManagerAO::on_port_event, this);
        return Q_HANDLED();

    case Q_EXIT_SIG:
        m_bluetooth.stop();
        m_ledBlue.setPinLow();
        return Q_HANDLED();

    case BT_CONNECTED_SIG:
        return tran(&connected);
    }
    return super(&enabled);
}

Q_STATE_DEF(BluetoothManagerAO, connected)
{
    switch (e->sig) {
    case Q_ENTRY_SIG:
        ledsOff(m_ledRed, m_ledBlue, m_ledGreen);
        m_ledGreen.setPinHigh();
        return Q_HANDLED();

    case Q_EXIT_SIG:
        m_ledGreen.setPinLow();
        return Q_HANDLED();

    case BT_DISCONNECTED_SIG:
        return tran(&advertising);

    case BT_DATA_SIG: {
        auto const *data = static_cast<BtDataEvt const *>(e);
        handle_json(data->data);
        return Q_HANDLED();
    }
    }
    return super(&enabled);
}

void BluetoothManagerAO::on_port_event(
    void *context,
    runtime::ports::BluetoothPort::Event event,
    char const *data)
{
    auto *actor = static_cast<BluetoothManagerAO *>(context);
    actor->handle_port_event(event, data);
}

void BluetoothManagerAO::handle_port_event(
    runtime::ports::BluetoothPort::Event event,
    char const *data)
{
    switch (event) {
    case runtime::ports::BluetoothPort::Event::Connected:
        QP::QF::PUBLISH(Q_NEW(QP::QEvt, BT_CONNECTED_SIG), this);
        break;

    case runtime::ports::BluetoothPort::Event::Disconnected:
        QP::QF::PUBLISH(Q_NEW(QP::QEvt, BT_DISCONNECTED_SIG), this);
        break;

    case runtime::ports::BluetoothPort::Event::Data: {
        auto *eventData = Q_NEW(BtDataEvt, BT_DATA_SIG);
        auto const length = std::min(std::strlen(data), sizeof(eventData->data) - 1U);
        std::memcpy(eventData->data, data, length);
        eventData->data[length] = '\0';
        QP::QF::PUBLISH(eventData, this);
        break;
    }
    }
}

void BluetoothManagerAO::handle_json(char const *data)
{
    (void)data;
    auto *event = Q_NEW(WifiCredentialsEvt, WIFI_CREDENTIALS_SIG);
    std::strncpy(event->ssid, "demo", sizeof(event->ssid) - 1U);
    std::strncpy(event->password, "demo", sizeof(event->password) - 1U);
    event->ssid[sizeof(event->ssid) - 1U] = '\0';
    event->password[sizeof(event->password) - 1U] = '\0';
    QP::QF::PUBLISH(event, this);
    QP::QF::PUBLISH(Q_NEW(QP::QEvt, BT_STOP_SIG), this);
}
