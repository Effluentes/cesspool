#pragma once

#include <runtime/application/LogPort.hh>

class Esp32Logger final
{
public:
    void write(runtime::application::LogLevel level, std::string_view message) noexcept;
};
