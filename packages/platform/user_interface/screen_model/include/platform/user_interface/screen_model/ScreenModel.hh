#pragma once

#include <array>
#include <cstdint>
#include <string_view>
#include <variant>
#include <expected>

namespace platform::user_interface
{
    struct MenuItem
    {
        std::string_view label;
    };

    struct MenuModel
    {
        static inline constexpr std::uint8_t maxMenuItems = 6;
        std::array<MenuItem, maxMenuItems> items;
        std::uint8_t itemCount;
        std::uint8_t selectedIndex;
    };

    struct StatusModel
    {
        std::uint16_t waterLevelMm;
        bool bluetoothConnected;
    };

    struct SlaveDetailModel
    {
        std::uint8_t slaveId;
        std::uint16_t waterLevelMm;
        bool online;
    };

    struct BleConfigModel
    {
        bool advertising;
        std::string_view deviceName;
    };

    using ScreenModel = std::variant<
        MenuModel,
        StatusModel,
        SlaveDetailModel,
        BleConfigModel>;

enum class DisplayError
{
    None = 0,
    InvalidModel,
    RenderFailed
};

using DisplayResult = std::expected<void, DisplayError>;
}