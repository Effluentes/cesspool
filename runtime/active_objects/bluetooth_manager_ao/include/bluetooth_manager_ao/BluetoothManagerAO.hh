#pragma once

#include "qpcpp.hpp"
#include <qpcpp/EspIdfPort.hh>
#include <qpcpp/Signals.hh>
#include "platform/driver/gpio/interface/GPIO.hh"
#include <cstdint>

struct BtDataEvt : QP::QEvt
{
    char data[128];
};

struct WifiCredentialsEvt : QP::QEvt
{
    char ssid[32];
    char password[64];
};

class BluetoothManagerAO : public QP::QActive
{
public:
    // DI przez konstruktor — zero singleton
    BluetoothManagerAO(
        platform::driver::interface::GPIO &ledRed,
        platform::driver::interface::GPIO &ledBlue,
        platform::driver::interface::GPIO &ledGreen);

protected:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(disabled);
    Q_STATE_DECL(enabled);
    Q_STATE_DECL(advertising);
    Q_STATE_DECL(connected);

private:
    void ble_start();
    void ble_stop();
    void ble_cleanup();
    void handle_json(const char *data);

    // GAP callback — wołany z kontekstu NimBLE, nie AO
    void on_gap_event(struct ble_gap_event *event);
    static int gap_event_cb(struct ble_gap_event *event, void *arg);

    platform::driver::interface::GPIO &m_ledRed;
    platform::driver::interface::GPIO &m_ledBlue;
    platform::driver::interface::GPIO &m_ledGreen;

    uint16_t m_connHandle = 0;
};