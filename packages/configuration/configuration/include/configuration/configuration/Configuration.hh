#pragma once
#include <string>

namespace configuration
{
    struct WifiConfiguration
    {
        std::string ssid;
        std::string password;
    };
    struct Configuration
    {
        WifiConfiguration wifi;
    };
}