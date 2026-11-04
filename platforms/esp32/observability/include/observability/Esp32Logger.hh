#pragma once

#include <observability/logs/interface/Logger.hh>

class Esp32Logger : public observability::Logger
{
public:
    ~Esp32Logger() override = default;
    Logger &getLogger(const std::string &name) override;
    void log(const std::string &message, const observability::Severity severity) override;
    void trace(const std::string &message) override;
    void debug(const std::string &message) override;
    void info(const std::string &message) override;
    void warning(const std::string &message) override;
    void error(const std::string &message) override;
    void critical(const std::string &message) override;

private:
    std::string loggerName_;
    Esp32Logger(const Esp32Logger &);
};