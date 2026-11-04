#include "observability/Esp32Logger.hh"

Esp32Logger::Esp32Logger(const std::string &name) : loggerName_(name) {}
Esp32Logger::~Esp32Logger() = default;

observability::Logger &Esp32Logger::getLogger(const std::string &name)
{
    static std::unordered_map<std::string, std::unique_ptr<NamedLogger>> instances;
    static std::mutex mutex;

    std::lock_guard<std::mutex> lock(mutex);
    auto &instance = instances[name];
    if (!instance)
    {
        instance = std::unique_ptr<NamedLogger>(new NamedLogger(name));
    }
    return *instance;
}

void Esp32Logger::log(const std::string &message, const observability::Severity severity)
{
    switch (severity)
    {
    case observability::Severity::INFO:
        ESP_LOGI(loggerName_.c_str(), "%s", message.c_str());
        break;
    case observability::Severity::TRACE:
        ESP_LOGV(loggerName_.c_str(), "%s", message.c_str());
        break;
    case observability::Severity::DEBUG:
        ESP_LOGD(loggerName_.c_str(), "%s", message.c_str());
        break;
    case observability::Severity::WARNING:
        ESP_LOGW(loggerName_.c_str(), "%s", message.c_str());
        break;
    case observability::Severity::ERROR:
        ESP_LOGE(loggerName_.c_str(), "%s", message.c_str());
        break;
    case observability::Severity::CRITICAL:
        ESP_LOGE(loggerName_.c_str(), "CRITICAL: %s", message.c_str());
        break;
    default:
        ESP_LOGI(loggerName_.c_str(), "%s", message.c_str());
        break;
    }
}

void Esp32Logger::trace(const std::string &message)
{
    log(message, observability::Severity::TRACE);
}

void Esp32Logger::debug(const std::string &message)
{
    log(message, observability::Severity::DEBUG);
}

void Esp32Logger::info(const std::string &message)
{
    log(message, observability::Severity::INFO);
}

void Esp32Logger::warning(const std::string &message)
{
    log(message, observability::Severity::WARNING);
}

void Esp32Logger::error(const std::string &message)
{
    log(message, observability::Severity::ERROR);
}

void Esp32Logger::critical(const std::string &message)
{
    log(message, observability::Severity::CRITICAL);
}
