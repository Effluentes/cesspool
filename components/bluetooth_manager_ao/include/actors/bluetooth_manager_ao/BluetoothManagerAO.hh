#pragma once

#include "qpcpp.hpp"

// NimBLE
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"

// ======================================================
// SIGNALS
// ======================================================
enum BtSignals : QP::QSignal {
    BT_START_SIG = QP::Q_USER_SIG,
    BT_STOP_SIG,
    BT_CONNECTED_SIG,
    BT_DISCONNECTED_SIG,
    BT_DATA_SIG,
    BT_MAX_SIG   // marker – przydatny do sprawdzania zakresu
};

// ======================================================
// EVENTS
// ======================================================
struct WifiCredentialsEvt : public QP::QEvt {
    char ssid[32]{};
    char password[64]{};
};

struct BtDataEvt : public QP::QEvt {
    char data[128]{};
};

// ======================================================
// ACTIVE OBJECT
// ======================================================
class BluetoothManagerAO : public QP::QActive {
public:
    static BluetoothManagerAO& getInstance();  // ✅ lazy init

    // NimBLE GAP callback bridge – wołany z kontekstu stosu NimBLE
    void on_gap_event(struct ble_gap_event* event);

protected:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(disabled);
    Q_STATE_DECL(enabled);       // ⬅️ NOWY stan nadrzędny
    Q_STATE_DECL(advertising);
    Q_STATE_DECL(connected);

private:
    void ble_start();
    void ble_stop();
    void ble_cleanup();           // ⬅️ NOWA metoda – pełne wyłączenie BLE
    void handle_json(const char* data);

private:
    BluetoothManagerAO();  // ✅ prywatny konstruktor
    uint16_t conn_handle = 0;     // ⬅️ inicjalizacja w deklaracji
};
//bluetoothQueue
 static QP::QEvt const* bluetoothQueue[10];