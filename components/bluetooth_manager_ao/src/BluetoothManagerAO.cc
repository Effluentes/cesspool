#include "actors/bluetooth_manager_ao/BluetoothManagerAO.hh"
#include "esp_log.h"
#include <cstring>
#include <algorithm>

Q_DEFINE_THIS_FILE

static const char* TAG = "BluetoothManagerAO";

// ======================================================
// SIGNALS (w signals.h – tu tylko przypomnienie)
// ======================================================
// BT_START_SIG, BT_STOP_SIG, BT_CONNECTED_SIG,
// BT_DISCONNECTED_SIG, BT_DATA_SIG

BluetoothManagerAO& BluetoothManagerAO::getInstance() {
    static BluetoothManagerAO instance;  // ✅ stworzony raz, przy pierwszym wywołaniu
    return instance;
}

BluetoothManagerAO::BluetoothManagerAO()
: QActive(&initial),
  conn_handle(0)
{}

// ======================================================
// INITIAL – pseudostan przejściowy
// ======================================================
Q_STATE_DEF(BluetoothManagerAO, initial)
{
    ESP_LOGI(TAG, "initial → disabled");
    (void)e;
    subscribe(BT_START_SIG);
    subscribe(BT_STOP_SIG);
    subscribe(BT_CONNECTED_SIG);
    subscribe(BT_DISCONNECTED_SIG);
    subscribe(BT_DATA_SIG);
    return tran(&disabled);
}
// ======================================================
// DISABLED – BLE wyłączone, czekamy na BT_START_SIG
// ======================================================
Q_STATE_DEF(BluetoothManagerAO, disabled)
{
    switch (e->sig) {
    case BT_START_SIG:
        return tran(&enabled);
    }
    return super(&QHsm::top);
}

// ======================================================
// ENABLED – stan nadrzędny: BLE może działać
// Obsługuje wspólne zdarzenia dla advertising i connected
// ======================================================
Q_STATE_DEF(BluetoothManagerAO, enabled)
{
    switch (e->sig) {

    case Q_INIT_SIG:                          // ← to właśnie brakowało
        return tran(&advertising);       // domyślny pod-stan

    case Q_ENTRY_SIG:
        ESP_LOGI(TAG, "enabled ENTER");
        return Q_HANDLED();

    case BT_STOP_SIG:
        ESP_LOGI(TAG, "enabled → disabled");
        ble_cleanup();
        return tran(&disabled);

    case Q_EXIT_SIG:
        ESP_LOGI(TAG, "enabled EXIT");
        ble_cleanup();
        return Q_HANDLED();
    }

    return super(&QHsm::top);
}
// ======================================================
// ADVERTISING – dziedziczy z enabled
// ======================================================
Q_STATE_DEF(BluetoothManagerAO, advertising)
{
    switch (e->sig) {

    case Q_ENTRY_SIG:
        ESP_LOGI(TAG, "advertising ENTER – starting BLE advertisement");
        ble_start();
        return Q_HANDLED();

    case BT_CONNECTED_SIG:
        ESP_LOGI(TAG, "advertising → connected");
        return tran(&connected);

    case BT_STOP_SIG:
        // Przechwycone przez enabled – ale możemy dodać log
        ESP_LOGI(TAG, "advertising – stop requested, deleguję do enabled");
        break;  // pozwalamy enabled to obsłużyć

    case Q_EXIT_SIG:
        ESP_LOGI(TAG, "advertising EXIT – stopping advertisement");
        ble_stop();
        return Q_HANDLED();
    }

    return super(&enabled);
}

// ======================================================
// CONNECTED – dziedziczy z enabled
// ======================================================
Q_STATE_DEF(BluetoothManagerAO, connected)
{
    switch (e->sig) {

    case Q_ENTRY_SIG:
        ESP_LOGI(TAG, "connected ENTER – client attached");
        return Q_HANDLED();

    case BT_DISCONNECTED_SIG:
        ESP_LOGI(TAG, "connected → advertising (client disconnected)");
        return tran(&advertising);

    case BT_DATA_SIG:
    {
        ESP_LOGI(TAG, "connected – received data");
        auto const* d = static_cast<BtDataEvt const*>(e);
        handle_json(d->data);
        return Q_HANDLED();
    }

    case BT_STOP_SIG:
        ESP_LOGI(TAG, "connected – stop requested, deleguję do enabled");
        break;  // enabled to przechwyci

    case Q_EXIT_SIG:
        ESP_LOGI(TAG, "connected EXIT – client detached");
        return Q_HANDLED();
    }

    return super(&enabled);
}

// ======================================================
// BLE START
// ======================================================
void BluetoothManagerAO::ble_start()
{
    ESP_LOGI(TAG, "ble_start – configuring GAP advertising");

    ble_svc_gap_device_name_set("ESP32-Provision");

    struct ble_gap_adv_params adv_params{};
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    int rc = ble_gap_adv_start(
        BLE_OWN_ADDR_PUBLIC,
        nullptr,
        BLE_HS_FOREVER,
        &adv_params,
        [](ble_gap_event* event, void* arg) -> int {
            auto* self = static_cast<BluetoothManagerAO*>(arg);
            self->on_gap_event(event);
            return 0;
        },
        this
    );

    if (rc != 0) {
        ESP_LOGE(TAG, "ble_start – ble_gap_adv_start failed: %d", rc);
        QP::QF::PUBLISH(Q_NEW(QP::QEvt, BT_STOP_SIG), this);
    }
}

// ======================================================
// BLE STOP (tylko reklama)
// ======================================================
void BluetoothManagerAO::ble_stop()
{
    ESP_LOGI(TAG, "ble_stop – stopping advertisement");
    int rc = ble_gap_adv_stop();
    if (rc != 0) {
        ESP_LOGW(TAG, "ble_stop – ble_gap_adv_stop failed: %d", rc);
    }
}

// ======================================================
// BLE CLEANUP – pełne wyłączenie (adv + ewentualne rozłączenie)
// ======================================================
void BluetoothManagerAO::ble_cleanup()
{
    ESP_LOGI(TAG, "ble_cleanup – full BLE shutdown");

    // Zatrzymaj reklamę, jeśli aktywna
    ble_gap_adv_stop();

    // Jeśli jesteśmy połączeni – rozłącz
    if (conn_handle != 0) {
        ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        conn_handle = 0;
    }
}

// ======================================================
// GAP EVENT → QP PUBLISH (bridge NimBLE → QP)
// UWAGA: to leci w kontekście stosu NimBLE, nie tasku AO
// ======================================================
void BluetoothManagerAO::on_gap_event(struct ble_gap_event* event)
{
    switch (event->type) {

    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI(TAG, "GAP event: CONNECT, status=%d", event->connect.status);
        if (event->connect.status == 0) {
            conn_handle = event->connect.conn_handle;
            QP::QF::PUBLISH(
                Q_NEW(QP::QEvt, BT_CONNECTED_SIG),
                this
            );
        } else {
            ESP_LOGW(TAG, "Connection failed, staying in advertising");
        }
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(TAG, "GAP event: DISCONNECT");
        conn_handle = 0;
        QP::QF::PUBLISH(
            Q_NEW(QP::QEvt, BT_DISCONNECTED_SIG),
            this
        );
        break;

    case BLE_GAP_EVENT_NOTIFY_RX:
    {
        struct os_mbuf* om = event->notify_rx.om;
        uint16_t len = OS_MBUF_PKTLEN(om);

        ESP_LOGI(TAG, "GAP event: NOTIFY_RX, len=%d", len);

        auto* evt = Q_NEW(BtDataEvt, BT_DATA_SIG);
        size_t copy_len = std::min((size_t)len, sizeof(evt->data) - 1);
        os_mbuf_copydata(om, 0, copy_len, evt->data);
        evt->data[copy_len] = '\0';

        QP::QF::PUBLISH(evt, this);
        break;
    }

    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI(TAG, "GAP event: ADV_COMPLETE");
        // Jeśli skończyliśmy reklamować bez połączenia – wróć do disabled
        QP::QF::PUBLISH(
            Q_NEW(QP::QEvt, BT_STOP_SIG),
            this
        );
        break;

    default:
        ESP_LOGD(TAG, "GAP event: %d (unhandled)", event->type);
        break;
    }
}

// ======================================================
// JSON → Credentials
// ======================================================
void BluetoothManagerAO::handle_json(const char* data)
{
    ESP_LOGI(TAG, "handle_json: %s", data);

    // Tymczasowo stub – w finalnej wersji: cJSON / jsmn
    const char* ssid     = "demo";
    const char* password = "demo";

    auto* evt = Q_NEW(WifiCredentialsEvt, 0);
    std::strncpy(evt->ssid, ssid, sizeof(evt->ssid) - 1);
    std::strncpy(evt->password, password, sizeof(evt->password) - 1);

    QP::QF::PUBLISH(evt, this);

    // Po wysłaniu credentials – wyłącz BLE
    QP::QF::PUBLISH(Q_NEW(QP::QEvt, BT_STOP_SIG), this);
}