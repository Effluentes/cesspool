#include "esp_log.h"
#include "runtime/composite_root/App.hh"
#include "qpcpp.hpp"
#include <platform_driver_esp32/Traits.hh>

using AppEsp32 = App<platform::esp32::Esp32Traits>;

extern "C" void app_main() {
    App app = AppEsp32();
    app.run();
}