#include "platform/bsp/storage/nvs/NonVolatileStorage.hh"
#include "nvs_flash.h"

namespace platform::bsp
{
NonVolatileStorage::NonVolatileStorage()
{
}

NonVolatileStorage::~NonVolatileStorage()
{
    erase();
}

bool NonVolatileStorage::erase()
{
    // return nvs_flash_erase() == ESP_OK;
    return true;
}

StartupResult NonVolatileStorage::initialize()
{
    esp_err_t err = nvs_flash_init();
    if (err != ESP_OK) {
        erase();
        return std::unexpected(StartupError::STORAGE_INIT_FAILED);
    }
    return {};
}
}