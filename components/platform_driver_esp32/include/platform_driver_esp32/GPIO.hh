#pragma once
#include "platform/driver/gpio/interface/GPIO.hh"
#include "platform/driver/gpio/interface/GPIOBase.hh"
#include <driver/gpio.h>

// note: Kluczowe pytanie: czy masz gdzieś w projekcie miejsce gdzie wołasz GPIO przez CRTP bezpośrednio (nie przez GPIO& interfejs)?
// Czyli czy jest jakiś kod który wygląda tak:
// template <typename TGpio>
// void someDriver(TGpio& gpio) {
//     gpio.setPinHigh();  // zero vtable, compile-time dispatch
// }
class Esp32GPIODriver final
    : public  platform::driver::interface::GPIO
    , private platform::driver::interface::GPIOBase<Esp32GPIODriver>
{
public:
    explicit Esp32GPIODriver(gpio_num_t pin);

    void setPinHigh() override;
    void setPinLow()  override;
    bool readPin()    const override;

private:
    void setPinHighImpl();
    void setPinLowImpl();
    bool readPinImpl() const;

    gpio_num_t pin_;

    friend struct platform::driver::interface::GPIOBase<Esp32GPIODriver>;
};