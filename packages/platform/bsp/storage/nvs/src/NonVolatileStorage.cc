#include "platform/storage/nvs/NonVolatileStorage.hh"
#include <nvs_flash.h>

namespace platform::storage
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
    return nvs_flash_erase() == ESP_OK;
}

bool NonVolatileStorage::initialize()
{
    esp_err_t err = nvs_flash_init();
    if (err != ESP_OK) {
        erase();
        return false;
    }
    return true;
}
}