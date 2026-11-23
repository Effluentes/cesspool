#pragma once

#include <human_machine_interface/display/interface/Display.hh>
#include <qpcpp.hpp>

namespace runtime::actors
{
    class DisplayAO final : public QP::QActive
    {
    public:
        explicit DisplayAO(human_machine_interface::interface::Display &display) noexcept;

        DisplayAO(DisplayAO const &) = delete;
        DisplayAO &operator=(DisplayAO const &) = delete;

    private:
        human_machine_interface::interface::Display &display_;

        Q_STATE_DECL(initial);
        Q_STATE_DECL(splash);
        Q_STATE_DECL(menu);
        Q_STATE_DECL(firstConfiguration);
    };
} // namespace runtime::actors