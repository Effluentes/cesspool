#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "u8g2.h"
#include "esp_log.h"
#include <cmath>
#include <cstring>

static const char* TAG = "oled";
static i2c_master_bus_handle_t s_bus;
static i2c_master_dev_handle_t s_dev;

static uint8_t i2c_byte_cb(u8x8_t* u8x8, uint8_t msg,
                            uint8_t arg_int, void* arg_ptr)
{
    static uint8_t buf[256];
    static size_t  buf_len = 0;
    switch (msg) {
    case U8X8_MSG_BYTE_INIT:             break;
    case U8X8_MSG_BYTE_START_TRANSFER:   buf_len = 0; break;
    case U8X8_MSG_BYTE_SEND:
        memcpy(buf + buf_len, (uint8_t*)arg_ptr, arg_int);
        buf_len += arg_int;
        break;
    case U8X8_MSG_BYTE_END_TRANSFER:
        i2c_master_transmit(s_dev, buf, buf_len, 500);
        buf_len = 0;
        break;
    }
    return 1;
}

static uint8_t delay_cb(u8x8_t*, uint8_t msg, uint8_t arg_int, void*)
{
    if (msg == U8X8_MSG_DELAY_MILLI)
        vTaskDelay(pdMS_TO_TICKS(arg_int));
    return 1;
}

// ── Zegar analogowy ─────────────────────────────────────────────
static void draw_clock(u8g2_t* g, int cx, int cy, int r, int h, int m, int s)
{
    // tarcza
    u8g2_DrawCircle(g, cx, cy, r, U8G2_DRAW_ALL);
    u8g2_DrawCircle(g, cx, cy, 2, U8G2_DRAW_ALL);

    // znaczniki godzin
    for (int i = 0; i < 12; i++) {
        float a = i * M_PI / 6.0f;
        int x0 = cx + (int)((r - 3) * sinf(a));
        int y0 = cy - (int)((r - 3) * cosf(a));
        int x1 = cx + (int)(r * sinf(a));
        int y1 = cy - (int)(r * cosf(a));
        u8g2_DrawLine(g, x0, y0, x1, y1);
    }

    // wskazówka godzinowa
    float ha = (h % 12 + m / 60.0f) * M_PI / 6.0f;
    u8g2_DrawLine(g, cx, cy,
                  cx + (int)((r * 0.55f) * sinf(ha)),
                  cy - (int)((r * 0.55f) * cosf(ha)));

    // wskazówka minutowa
    float ma = m * M_PI / 30.0f;
    u8g2_DrawLine(g, cx, cy,
                  cx + (int)((r * 0.80f) * sinf(ma)),
                  cy - (int)((r * 0.80f) * cosf(ma)));

    // wskazówka sekundowa (cieńsza — rysujemy dwa razy offset)
    float sa = s * M_PI / 30.0f;
    u8g2_DrawLine(g, cx, cy,
                  cx + (int)((r * 0.90f) * sinf(sa)),
                  cy - (int)((r * 0.90f) * cosf(sa)));
}

// ── Pasek postępu ───────────────────────────────────────────────
static void draw_progress(u8g2_t* g, int x, int y, int w, int h, int pct)
{
    u8g2_DrawFrame(g, x, y, w, h);
    int fill = (w - 2) * pct / 100;
    if (fill > 0)
        u8g2_DrawBox(g, x + 1, y + 1, fill, h - 2);
}

// ── Scrollujący tekst ───────────────────────────────────────────
static void draw_scroll(u8g2_t* g, const char* text, int offset)
{
    u8g2_SetFont(g, u8g2_font_5x7_tr);
    int text_w = u8g2_GetStrWidth(g, text);
    int x = 128 - (offset % (text_w + 128));
    u8g2_DrawStr(g, x, 63, text);
}

extern "C" void app_main()
{
    // I2C init
    i2c_master_bus_config_t bus_cfg = {
        .i2c_port          = I2C_NUM_0,
        .sda_io_num        = GPIO_NUM_1,
        .scl_io_num        = GPIO_NUM_2,
        .clk_source        = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags = { .enable_internal_pullup = true },
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &s_bus));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address  = 0x3C,
        .scl_speed_hz    = 400000,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(s_bus, &dev_cfg, &s_dev));

    // u8g2 init
    u8g2_t u8g2;
    u8g2_Setup_sh1106_i2c_128x64_noname_f(&u8g2, U8G2_R0, i2c_byte_cb, delay_cb);
    u8x8_SetI2CAddress(&u8g2.u8x8, 0x3C);
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);

    // symulowany czas startowy
    int h = 10, m = 9, s = 0;
    int scroll_offset = 0;
    int pct = 0;

    const char* scroll_text = "  cesspool v1.0  |  ESP32-C3  |  water level OK  ";

    while (true) {
        u8g2_ClearBuffer(&u8g2);

        // zegar — lewa część (r=28, cx=30, cy=32)
        draw_clock(&u8g2, 30, 32, 28, h, m, s);

        // separator
        u8g2_DrawVLine(&u8g2, 62, 0, 54);

        // prawa część — dane
        u8g2_SetFont(&u8g2, u8g2_font_5x7_tr);
        char buf[32];

        // czas cyfrowy
        snprintf(buf, sizeof(buf), "%02d:%02d:%02d", h, m, s);
        u8g2_DrawStr(&u8g2, 66, 10, buf);

        // etykiety + wartości
        u8g2_DrawStr(&u8g2, 66, 22, "LEVEL:");
        snprintf(buf, sizeof(buf), "%3d%%", pct);
        u8g2_DrawStr(&u8g2, 100, 22, buf);

        // pasek poziomu wody
        draw_progress(&u8g2, 66, 26, 58, 8, pct);

        u8g2_DrawStr(&u8g2, 66, 42, "TEMP:  23.4C");
        u8g2_DrawStr(&u8g2, 66, 52, "SLAVE: OK");

        // scrollujący tekst na dole
        draw_scroll(&u8g2, scroll_text, scroll_offset);

        u8g2_SendBuffer(&u8g2);

        // aktualizacja czasu
        s++;
        if (s >= 60) { s = 0; m++; }
        if (m >= 60) { m = 0; h++; }
        if (h >= 24)   h = 0;

        // symulacja poziomu wody
        pct = (pct + 1) % 101;

        scroll_offset += 2;

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}