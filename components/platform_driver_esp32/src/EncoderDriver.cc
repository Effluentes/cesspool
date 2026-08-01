#include "platform_driver_esp32/EncoderDriver.hh"
#include <qpcpp/Signals.hh>

EncoderDriver::EncoderDriver(gpio_num_t pinA,
                             gpio_num_t pinB,
                             gpio_num_t pinBtn) noexcept
    : pinA_{pinA}, pinB_{pinB}, pinBtn_{pinBtn}
{
}

void EncoderDriver::configureGPIO()
{
    gpio_config_t encoder = {
        .pin_bit_mask = (1ULL << pinA_) | (1ULL << pinB_) | (1ULL << pinBtn_),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE};
    gpio_config(&encoder);
}

void EncoderDriver::applyISRCallback()
{
    gpio_install_isr_service(0);
    gpio_isr_handler_add(pinA_, onISRCallback, this);
    gpio_isr_handler_add(pinB_, onISRCallback, this);
    gpio_isr_handler_add(pinBtn_, onISRCallback, this);
}

void EncoderDriver::initialize()
{
    configureGPIO();
    applyISRCallback();
    startPollTask("enc_poll");
}

void IRAM_ATTR EncoderDriver::onISRCallback(void *arg)
{
    static_cast<EncoderDriver *>(arg)->handleIsr();
}

void IRAM_ATTR EncoderDriver::handleIsr()
{
    pushFromISR({
        static_cast<uint8_t>(gpio_get_level(pinA_) != 0),
        static_cast<uint8_t>(gpio_get_level(pinB_) != 0),
        static_cast<uint8_t>(gpio_get_level(pinBtn_) != 0),
    });
    BaseType_t woken = pdFALSE;
    vTaskNotifyGiveFromISR(taskHandle_, &woken);
    portYIELD_FROM_ISR(woken);
}

void EncoderDriver::processSample(const EncoderRawSample &sample) noexcept
{
    // auto const ev = debouncer_.update(sample.a, sample.b, sample.btn);
    // publishEvent(ev.direction);

    publishEvent(EncoderNavDirection::None);
}

void EncoderDriver::publishEvent(EncoderNavDirection dir) noexcept
{
    auto *evt = Q_NEW(EncoderEvt, Signals::ENCODER_NAV_SIG);
    evt->navDirection = dir;
    QP::QF::PUBLISH(evt, this);
}