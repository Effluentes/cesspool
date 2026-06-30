#pragma once
#include "qpcpp.hpp"

extern "C" Q_NORETURN Q_onAssert(char_t const * const module, int_t location);

enum BluetoothSignals : QP::QSignal {
    BT_START_SIG = QP::Q_USER_SIG,
    BT_STOP_SIG,
    BT_CONNECTED_SIG,
    BT_DISCONNECTED_SIG,
    BT_DATA_SIG,
    WIFI_CREDENTIALS_SIG,
    RADIO_INIT_SIG,
    RADIO_DATA_RX_SIG,
    RADIO_SEND_SIG,
    RADIO_TX_TIMEOUT_SIG,
    SENSOR_DATA_SIG,       // publikowany dalej na bus
    // ...
};