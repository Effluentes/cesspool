#include "driver/spi_master.h"
#include "esp_err.h"
#include <cstdint>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <cstring>
#include <vector>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include <iostream>
#include "driver/spi_master.h"
#include "esp_err.h"
#include <cstdint>
#include <vector>
#include "esp_log.h"

namespace radio
{

    class CC1101
    {
    public:
        struct Config
        {
            spi_host_device_t host = SPI2_HOST;
            gpio_num_t sclk_pin;
            gpio_num_t mosi_pin;
            gpio_num_t miso_pin;
            gpio_num_t csn_pin;
            gpio_num_t gdo0_pin;                // -1 jeśli nieużywany
            int spi_clock_hz = 2 * 1000 * 1000; // 2 MHz
        };

        explicit CC1101(const Config &cfg);
        esp_err_t init();
        esp_err_t sendPacket(const uint8_t *data, size_t len);
        int receivePacket(uint8_t *buf, size_t max_len);
        void setRx();
        void setTx();

    private:
        Config cfg_;
        spi_device_handle_t spi_;
        esp_err_t reset();
        esp_err_t writeReg(uint8_t addr, uint8_t value);
        esp_err_t readReg(uint8_t addr, uint8_t &value);
        esp_err_t burstWrite(uint8_t addr, const uint8_t *data, size_t len);
        esp_err_t burstRead(uint8_t addr, uint8_t *data, size_t len);
        esp_err_t strobe(uint8_t cmd);
        void setDefaultConfig();
    };

} // namespace radio

namespace radio
{

    static const char *TAG = "CC1101";

#define CC1101_WRITE_SINGLE 0x00
#define CC1101_READ_SINGLE 0x80
#define CC1101_WRITE_BURST 0x40
#define CC1101_READ_BURST 0xC0

#define CC1101_SRES 0x30
#define CC1101_SRX 0x34
#define CC1101_STX 0x35
#define CC1101_SIDLE 0x36
#define CC1101_RXBYTES 0x3B
#define CC1101_FIFO 0x3F

    CC1101::CC1101(const Config &cfg) : cfg_(cfg), spi_(nullptr) {}

    esp_err_t CC1101::init()
    {
        spi_bus_config_t buscfg = {};
        buscfg.mosi_io_num = cfg_.mosi_pin;
        buscfg.miso_io_num = cfg_.miso_pin;
        buscfg.sclk_io_num = cfg_.sclk_pin;
        buscfg.max_transfer_sz = 64;

        spi_device_interface_config_t devcfg = {};
        devcfg.clock_speed_hz = cfg_.spi_clock_hz;
        devcfg.mode = 0;
        devcfg.spics_io_num = cfg_.csn_pin;
        devcfg.queue_size = 1;

        ESP_ERROR_CHECK(spi_bus_initialize(cfg_.host, &buscfg, SPI_DMA_CH_AUTO));
        ESP_ERROR_CHECK(spi_bus_add_device(cfg_.host, &devcfg, &spi_));

        ESP_LOGI(TAG, "SPI bus initialized");

        reset();
        setDefaultConfig();

        uint8_t ver = 0;
        readReg(0x31, ver);
        ESP_LOGI(TAG, "CC1101 VERSION: 0x%02X", ver);

        return ESP_OK;
    }

    esp_err_t CC1101::reset() { return strobe(CC1101_SRES); }

    esp_err_t CC1101::strobe(uint8_t cmd)
    {
        spi_transaction_t t{};
        t.length = 8;
        t.flags = SPI_TRANS_USE_TXDATA;
        t.tx_data[0] = cmd;
        return spi_device_transmit(spi_, &t);
    }

    esp_err_t CC1101::writeReg(uint8_t addr, uint8_t value)
    {
        uint8_t data[2] = {static_cast<uint8_t>(addr | CC1101_WRITE_SINGLE), value};
        spi_transaction_t t{};
        t.length = 16;
        t.tx_buffer = data;
        return spi_device_transmit(spi_, &t);
    }

    esp_err_t CC1101::readReg(uint8_t addr, uint8_t &value)
    {
        uint8_t tx[2] = {static_cast<uint8_t>(addr | CC1101_READ_SINGLE), 0};
        uint8_t rx[2] = {};
        spi_transaction_t t{};
        t.length = 16;
        t.tx_buffer = tx;
        t.rx_buffer = rx;
        auto ret = spi_device_transmit(spi_, &t);
        value = rx[1];
        return ret;
    }

    esp_err_t CC1101::burstWrite(uint8_t addr, const uint8_t *data, size_t len)
    {
        std::vector<uint8_t> buf(len + 1);
        buf[0] = addr | CC1101_WRITE_BURST;
        memcpy(&buf[1], data, len);
        spi_transaction_t t{};
        t.length = (len + 1) * 8;
        t.tx_buffer = buf.data();
        return spi_device_transmit(spi_, &t);
    }

    esp_err_t CC1101::burstRead(uint8_t addr, uint8_t *data, size_t len)
    {
        std::vector<uint8_t> tx(len + 1), rx(len + 1);
        tx[0] = addr | CC1101_READ_BURST;
        spi_transaction_t t{};
        t.length = (len + 1) * 8;
        t.tx_buffer = tx.data();
        t.rx_buffer = rx.data();
        auto ret = spi_device_transmit(spi_, &t);
        memcpy(data, &rx[1], len);
        return ret;
    }

    void CC1101::setDefaultConfig()
    {
        // 433 MHz, 38.4kbps, 2-FSK
        writeReg(0x0B, 0x0C);
        writeReg(0x0C, 0x00);
        writeReg(0x0D, 0x10);
        writeReg(0x0E, 0xA7);
        writeReg(0x0F, 0x62);
        writeReg(0x10, 0x5D);
        writeReg(0x11, 0x93);
        writeReg(0x12, 0x13);
        writeReg(0x15, 0x22);
        writeReg(0x18, 0x18);
        writeReg(0x19, 0x16);
        writeReg(0x1B, 0x43);
        writeReg(0x21, 0xB6);
        writeReg(0x23, 0xE9);
        writeReg(0x24, 0x2A);
        writeReg(0x25, 0x00);
        writeReg(0x26, 0x1F);
        writeReg(0x29, 0x59);
        writeReg(0x2C, 0x81);
        writeReg(0x2D, 0x35);
        writeReg(0x2E, 0x09);

        // SYNC word
        writeReg(0x04, 0x45);
        writeReg(0x05, 0x56);

        // Packet settings
        writeReg(0x06, 0x04);
        writeReg(0x07, 0x00); // fixed length, no CRC
        writeReg(0x08, 0x10); // length = 16
    }

    void CC1101::setRx() { strobe(CC1101_SRX); }
    void CC1101::setTx() { strobe(CC1101_STX); }

    esp_err_t CC1101::sendPacket(const uint8_t *data, size_t len)
    {
        strobe(CC1101_SIDLE);
        burstWrite(CC1101_FIFO, data, len);
        setTx();
        vTaskDelay(pdMS_TO_TICKS(50));
        setRx();
        return ESP_OK;
    }

    int CC1101::receivePacket(uint8_t *buf, size_t max_len)
    {
        uint8_t rxbytes = 0;
        readReg(CC1101_RXBYTES, rxbytes);
        if (rxbytes == 0)
            return 0;
        if (rxbytes > max_len)
            rxbytes = max_len;
        burstRead(CC1101_FIFO, buf, rxbytes);
        setRx();
        return rxbytes;
    }

} // namespace radio



extern "C" void app_main()
{
    static const char *TAG = "master";

    radio::CC1101::Config cfg{
        .sclk_pin = GPIO_NUM_4,
        .mosi_pin = GPIO_NUM_5,
        .miso_pin = GPIO_NUM_2,
        .csn_pin = GPIO_NUM_0,
        .gdo0_pin = GPIO_NUM_3};

    radio::CC1101 radio(cfg);
    radio.init();

    uint8_t buf[32];
    const char msg[] = "PING";

    while (true)
    {
        ESP_LOGI(TAG, "Sending: %s", msg);
        radio.sendPacket((const uint8_t *)msg, sizeof(msg));

        vTaskDelay(pdMS_TO_TICKS(200));
        int len = radio.receivePacket(buf, sizeof(buf));
        if (len > 0)
        {
            buf[len] = '\0';
            ESP_LOGI(TAG, "Received: %s", buf);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
