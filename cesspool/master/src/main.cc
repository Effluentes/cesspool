#include <platform/bsp/startup/StartupBuilder.hh>
#include <platform/bsp/storage/nvs/NonVolatileStorage.hh>

extern "C" void app_main() {
platform::bsp::NonVolatileStorage nvs;
   platform::bsp::StartupBuilder startupBuilder;
    auto startupResult =startupBuilder
        .withStorage(nvs)
        .initialize();

    if (not startupResult) {
    }
}
