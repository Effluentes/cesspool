// #include <stdio.h>
// #include <string.h>

// #include "nvs_flash.h"
// #include "esp_log.h"

// #include "esp_nimble_hci.h"
// #include "nimble/nimble_port.h"
// #include "nimble/nimble_port_freertos.h"

// #include "host/ble_hs.h"
// #include "services/gap/ble_svc_gap.h"
// #include "services/gatt/ble_svc_gatt.h"
// static uint16_t led_chr_val_handle;

// static const char *TAG = "BLE_SIMPLE";
// static uint8_t ble_addr_type;

// // ===== UUIDy =====
// static const ble_uuid128_t svc_uuid = BLE_UUID128_INIT(
//     0xAA,0xAA,0xAA,0xAA,
//     0xAA,0xAA,0xAA,0xAA,
//     0xAA,0xAA,0xAA,0xAA,
//     0xAA,0xAA,0xAA,0xAA
// );

// static const ble_uuid128_t chr_read_uuid = BLE_UUID128_INIT(
//     0xBB,0xBB,0xBB,0xBB,
//     0xBB,0xBB,0xBB,0xBB,
//     0xBB,0xBB,0xBB,0xBB,
//     0xBB,0xBB,0xBB,0x01
// );

// static const ble_uuid128_t chr_write_uuid = BLE_UUID128_INIT(
//     0xCC,0xCC,0xCC,0xCC,
//     0xCC,0xCC,0xCC,0xCC,
//     0xCC,0xCC,0xCC,0xCC,
//     0xCC,0xCC,0xCC,0x02
// );

// // ===== CALLBACKY =====
// static int
// read_cb(uint16_t conn_handle, uint16_t attr_handle,
//         struct ble_gatt_access_ctxt *ctxt, void *arg)
// {
//     ESP_LOGI(TAG, "ACCESS OP = %d", ctxt->op);
//     const char reply[] = "ESP32 READ OK";
//     os_mbuf_append(ctxt->om, reply, strlen(reply));
//     ESP_LOGI(TAG, "Client READ");
//     return 0;
// }

// static int
// write_cb(uint16_t conn_handle, uint16_t attr_handle,
//          struct ble_gatt_access_ctxt *ctxt, void *arg)
// {
//     ESP_LOGI(TAG, "WRITE event: op=%d handle=%d",
//              ctxt->op, attr_handle);

//     if (ctxt->op != BLE_GATT_ACCESS_OP_WRITE_CHR) {
//         ESP_LOGW(TAG, "Not a write op");
//         return BLE_ATT_ERR_UNLIKELY;
//     }

//     char buf[100] = {0};
//     int len = OS_MBUF_PKTLEN(ctxt->om);

//     if (len > sizeof(buf) - 1) {
//         ESP_LOGE(TAG, "Too large payload");
//         return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
//     }

//     ble_hs_mbuf_to_flat(ctxt->om, buf, len, NULL);
//     buf[len] = 0;

//     ESP_LOGI(TAG, "Received (%d bytes): %s", len, buf);

//     return 0;
// }


// // ===== DEFINICJA GATT =====
//  const struct ble_gatt_chr_def gatt_chars[] = {
//     {
//         .uuid = &chr_read_uuid.u,
//         .access_cb = read_cb,
//         .flags = BLE_GATT_CHR_F_READ,
//     },
//     {
//         .uuid = &chr_write_uuid.u,
//         .access_cb = write_cb,
//         .flags =  BLE_GATT_CHR_F_WRITE
//        | BLE_GATT_CHR_F_WRITE_NO_RSP
//        | BLE_GATT_CHR_F_READ,
//        .val_handle = &led_chr_val_handle
//     },
//     {0}
// };

//  const struct ble_gatt_svc_def gatt_svcs[] = {
//     {
//         .type = BLE_GATT_SVC_TYPE_PRIMARY,
//         .uuid = &svc_uuid.u,
//         .characteristics = gatt_chars,
//     },
//     {0}
// };

// // ===== GAP + ADVERTISING =====
// static void advertise(void);

// static int
// gap_event(struct ble_gap_event *event, void *arg)
// {
//     switch (event->type) {
//     case BLE_GAP_EVENT_CONNECT:
//         ESP_LOGI(TAG, "Connected");
//         break;
//     case BLE_GAP_EVENT_DISCONNECT:
//         ESP_LOGI(TAG, "Disconnected");
//         advertise();
//         break;
//     default:
//         break;
//     }
//     return 0;
// }

// static void advertise(void)
// {
//     struct ble_hs_adv_fields fields = {0};
//     const char *name = "ESP32_SIMPLE";

//     fields.name = (uint8_t*)name;
//     fields.name_len = strlen(name);
//     fields.name_is_complete = 1;

//     ble_gap_adv_set_fields(&fields);

//     struct ble_gap_adv_params adv_params = {0};
//     adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
//     adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

//     ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER,
//                       &adv_params, gap_event, NULL);

//     ESP_LOGI(TAG, "Advertising started");
// }

// static void on_sync(void)
// {
//     ble_hs_id_infer_auto(0, &ble_addr_type);
//     advertise();
// }

// static void host_task(void *param)
// {
//     nimble_port_run();
// }

// extern "C" void app_main(void)
// {
//     ESP_ERROR_CHECK(nvs_flash_init());
//     ESP_ERROR_CHECK(nimble_port_init());

//     ESP_ERROR_CHECK(ble_svc_gap_device_name_set("ESP32_SIMPLE"));
//     ble_svc_gap_init();
//     ble_svc_gatt_init();

//     ESP_ERROR_CHECK(ble_gatts_count_cfg(gatt_svcs));
//     ESP_ERROR_CHECK(ble_gatts_add_svcs(gatt_svcs));

//     ble_hs_cfg.sync_cb = on_sync;

//     nimble_port_freertos_init(host_task);
// }

#include <platform/storage/nvs/NonVolatileStorage.hh>
extern "C" int app_main(void)
{
    platform::storage::NonVolatileStorage nvs;
    nvs.initialize();
    return 0;
}