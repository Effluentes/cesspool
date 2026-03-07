#include <platform/bsp/startup/StartupBuilder.hh>

extern "C" void app_main() {
   platform::bsp::StartupBuilder startupBuilder;
    auto startupResult =startupBuilder
        .withNvs()
        .build();

    if (not startupResult) {
    }
}
