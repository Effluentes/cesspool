#include "bluetooth_manager_ao/BluetoothManagerAO.hh"
#include "esp_log.h"
#include "host/ble_gap.h"
#include "services/gap/ble_svc_gap.h"
#include <cstring>
#include <algorithm>
#include "nimble/nimble_port.h"
Q_DEFINE_THIS_FILE

static const char* TAG = "BluetoothManagerAO";

static void ledsOff(
    platform::driver::interface::GPIO& r,
    platform::driver::interface::GPIO& b,
    platform::driver::interface::GPIO& g)
{
    r.setPinLow();
    b.setPinLow();
    g.setPinLow();
}

BluetoothManagerAO::BluetoothManagerAO(
    platform::driver::interface::GPIO& ledRed,
    platform::driver::interface::GPIO& ledBlue,
    platform::driver::interface::GPIO& ledGreen)
    : QActive(Q_STATE_CAST(&initial))
    , m_ledRed(ledRed)
    , m_ledBlue(ledBlue)
    , m_ledGreen(ledGreen)
{}

// ── initial ───────────────────────────────────────────────────────

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

// ── disabled ──────────────────────────────────────────────────────

Q_STATE_DEF(BluetoothManagerAO, disabled)
{
    switch (e->sig) {

    case Q_ENTRY_SIG:
        ledsOff(m_ledRed, m_ledBlue, m_ledGreen);
        m_ledRed.setPinHigh();
        ESP_LOGI(TAG, "disabled ENTRY");
        return Q_HANDLED();

    case Q_EXIT_SIG:
        m_ledRed.setPinLow();
        return Q_HANDLED();

    case BT_START_SIG:
        return tran(&enabled);
    }
    return super(&QHsm::top);
}

// ── enabled (composite) ───────────────────────────────────────────

Q_STATE_DEF(BluetoothManagerAO, enabled)
{
    switch (e->sig) {

    case Q_INIT_SIG:
        return tran(&advertising);

    case Q_ENTRY_SIG:
        ESP_LOGI(TAG, "enabled ENTRY");
        return Q_HANDLED();

    case Q_EXIT_SIG:
        ble_cleanup();
        ledsOff(m_ledRed, m_ledBlue, m_ledGreen);
        ESP_LOGI(TAG, "enabled EXIT");
        return Q_HANDLED();

    case BT_STOP_SIG:
        return tran(&disabled);
    }
    return super(&QHsm::top);
}

// ── advertising ───────────────────────────────────────────────────

Q_STATE_DEF(BluetoothManagerAO, advertising)
{
    switch (e->sig) {

    case Q_ENTRY_SIG:
        ledsOff(m_ledRed, m_ledBlue, m_ledGreen);
        m_ledBlue.setPinHigh();
        ESP_LOGI(TAG, "advertising ENTRY");
        ble_start();
        return Q_HANDLED();

    case Q_EXIT_SIG:
        ble_stop();
        m_ledBlue.setPinLow();
        ESP_LOGI(TAG, "advertising EXIT");
        return Q_HANDLED();

    case BT_CONNECTED_SIG:
        return tran(&connected);
    }
    return super(&enabled);
}

// ── connected ────────────────────────────────────────────────────

Q_STATE_DEF(BluetoothManagerAO, connected)
{
    switch (e->sig) {

    case Q_ENTRY_SIG:
        ledsOff(m_ledRed, m_ledBlue, m_ledGreen);
        m_ledGreen.setPinHigh();
        ESP_LOGI(TAG, "connected ENTRY");
        return Q_HANDLED();

    case Q_EXIT_SIG:
        m_ledGreen.setPinLow();
        ESP_LOGI(TAG, "connected EXIT");
        return Q_HANDLED();

    case BT_DISCONNECTED_SIG:
        return tran(&advertising);

    case BT_DATA_SIG: {
        auto const* d = static_cast<BtDataEvt const*>(e);
        handle_json(d->data);
        return Q_HANDLED();
    }
    }
    return super(&enabled);
}

// ── BLE ──────────────────────────────────────────────────────────

int BluetoothManagerAO::gap_event_cb(struct ble_gap_event* event, void* arg)
{
    static_cast<BluetoothManagerAO*>(arg)->on_gap_event(event);
    return 0;
}

void BluetoothManagerAO::ble_start()
{
    int name_rc = ble_svc_gap_device_name_set("ESP32-Provision");
    if (name_rc != 0) {
        ESP_LOGE(TAG, "ble_svc_gap_device_name_set failed: %d", name_rc);
    } else {
        ESP_LOGI(TAG, "BLE device name set to: ESP32-Provision");
    }

    ESP_LOGI(TAG, "Starting BLE advertising as ESP32-Provision");

    const char *device_name = "ESP32_NIMBLE";
    struct ble_hs_adv_fields fields;
    memset(&fields, 0, sizeof(fields));
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.name = (uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;
    int rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        ESP_LOGE(TAG, "Błąd ustawiania pól rozgłoszeniowych: %d", rc);
        return;
    }

    struct ble_gap_adv_params adv_params{};
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

     rc = ble_gap_adv_start(
        BLE_OWN_ADDR_PUBLIC, nullptr, BLE_HS_FOREVER,
        &adv_params, gap_event_cb, this
    );

    if (rc != 0) {
        ESP_LOGE(TAG, "ble_gap_adv_start failed: %d", rc);
        QP::QF::PUBLISH(Q_NEW(QP::QEvt, BT_STOP_SIG), this);
    }
}

void BluetoothManagerAO::ble_stop()
{
    ble_gap_adv_stop();
}

void BluetoothManagerAO::ble_cleanup()
{
    ble_gap_adv_stop();
    if (m_connHandle != 0) {
        ble_gap_terminate(m_connHandle, BLE_ERR_REM_USER_CONN_TERM);
        m_connHandle = 0;
    }
}

void BluetoothManagerAO::on_gap_event(struct ble_gap_event* event)
{
    switch (event->type) {

    case BLE_GAP_EVENT_CONNECT:
        if (event->connect.status == 0) {
            m_connHandle = event->connect.conn_handle;
            QP::QF::PUBLISH(Q_NEW(QP::QEvt, BT_CONNECTED_SIG), this);
        }
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        m_connHandle = 0;
        QP::QF::PUBLISH(Q_NEW(QP::QEvt, BT_DISCONNECTED_SIG), this);
        break;

    case BLE_GAP_EVENT_NOTIFY_RX: {
        struct os_mbuf* om = event->notify_rx.om;
        uint16_t len = OS_MBUF_PKTLEN(om);
        auto* evt = Q_NEW(BtDataEvt, BT_DATA_SIG);
        size_t copy_len = std::min((size_t)len, sizeof(evt->data) - 1);
        os_mbuf_copydata(om, 0, copy_len, evt->data);
        evt->data[copy_len] = '\0';
        if (copy_len > 0) {
            ESP_LOGI(TAG, "Received BLE data (%zu bytes): %s", copy_len, evt->data);
        } else {
            ESP_LOGI(TAG, "Received BLE data (0 bytes)");
        }
        QP::QF::PUBLISH(evt, this);
        break;
    }

    case BLE_GAP_EVENT_ADV_COMPLETE:
        QP::QF::PUBLISH(Q_NEW(QP::QEvt, BT_STOP_SIG), this);
        break;
    }
}

void BluetoothManagerAO::handle_json(const char* data)
{
    ESP_LOGI(TAG, "Processing received BLE payload: %s", data);

    auto* evt = Q_NEW(WifiCredentialsEvt, WIFI_CREDENTIALS_SIG);
    std::strncpy(evt->ssid,     "demo", sizeof(evt->ssid)     - 1);
    std::strncpy(evt->password, "demo", sizeof(evt->password) - 1);
    QP::QF::PUBLISH(evt, this);

    QP::QF::PUBLISH(Q_NEW(QP::QEvt, BT_STOP_SIG), this);
}