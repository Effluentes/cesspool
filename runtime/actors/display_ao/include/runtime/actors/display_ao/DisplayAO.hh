#pragma once

#include <cstdint>
#include <human_machine_interface/display/interface/Display.hh>
#include <qpcpp.hpp>

namespace runtime::actors
{
    class DisplayAO final : public QP::QActive
    {
    public:
        explicit DisplayAO(human_machine_interface::display::interface::Display &display) noexcept;

        DisplayAO(DisplayAO const &) = delete;
        DisplayAO &operator=(DisplayAO const &) = delete;

    private:
        human_machine_interface::display::interface::Display &display_;
        std::uint8_t selectedSetting_{0};

        Q_STATE_DECL(initial);
        Q_STATE_DECL(waiting);
        Q_STATE_DECL(splash);
        Q_STATE_DECL(firstConfiguration);
        Q_STATE_DECL(menu);
        Q_STATE_DECL(slaves);
        Q_STATE_DECL(settings);
        Q_STATE_DECL(settingDetail);
    };
} // namespace runtime::actors