#pragma once

#include "qpcpp.hpp"

#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"

enum BtSignals : QP::QSignal {
    BT_START_SIG = QP::Q_USER_SIG,
    BT_STOP_SIG,
    BT_CONNECTED_SIG,
    BT_DISCONNECTED_SIG,
    BT_DATA_SIG,
    BT_MAX_SIG
};

struct WifiCredentialsEvt : public QP::QEvt {
    char ssid[32]{};
    char password[64]{};
};

struct BtDataEvt : public QP::QEvt {
    char data[128]{};
};

class BluetoothManagerAO : public QP::QActive {
public:
    static BluetoothManagerAO& getInstance();

    void on_gap_event(struct ble_gap_event* event);

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
    void handle_json(const char* data);

private:
    BluetoothManagerAO();
    uint16_t conn_handle = 0;
};
//bluetoothQueue
static QP::QEvt const* bluetoothQueue[10];