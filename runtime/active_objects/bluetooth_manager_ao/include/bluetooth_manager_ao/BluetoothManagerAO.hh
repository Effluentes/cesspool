#pragma once

#include "qpcpp.hpp"
#include <qpcpp/Signals.hh>
#include "platform/driver/gpio/interface/GPIO.hh"
#include "runtime/ports/BluetoothPort.hh"
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
        platform::driver::interface::GPIO &ledGreen,
        runtime::ports::BluetoothPort &bluetooth);

protected:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(disabled);
    Q_STATE_DECL(enabled);
    Q_STATE_DECL(advertising);
    Q_STATE_DECL(connected);

private:
    void handle_json(const char *data);
    static void on_port_event(
        void *context,
        runtime::ports::BluetoothPort::Event event,
        char const *data);
    void handle_port_event(
        runtime::ports::BluetoothPort::Event event,
        char const *data);

    platform::driver::interface::GPIO &m_ledRed;
    platform::driver::interface::GPIO &m_ledBlue;
    platform::driver::interface::GPIO &m_ledGreen;
    runtime::ports::BluetoothPort &m_bluetooth;
};