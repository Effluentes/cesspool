#include "observability/Esp32Logger.hh"

#include <esp_log.h>

#include <algorithm>
#include <limits>

void Esp32Logger::write(
    runtime::application::LogLevel level,
    std::string_view message) noexcept
{
    esp_log_level_t idfLevel = ESP_LOG_INFO;
    switch (level)
    {
    case runtime::application::LogLevel::info:
        idfLevel = ESP_LOG_INFO;
        break;
    case runtime::application::LogLevel::warning:
        idfLevel = ESP_LOG_WARN;
        break;
    case runtime::application::LogLevel::error:
        idfLevel = ESP_LOG_ERROR;
        break;
    }

    const auto length = std::min(
        message.size(),
        static_cast<std::size_t>(std::numeric_limits<int>::max()));
    const auto *text = message.empty() ? "" : message.data();
    esp_log_write(idfLevel, "cesspool", "%.*s", static_cast<int>(length), text);
}
