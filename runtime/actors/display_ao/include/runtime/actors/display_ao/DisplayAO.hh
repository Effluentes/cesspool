#pragma once

#include <runtime/application/Application.hh>
#include <runtime/application/DisplayPort.hh>
#include <qpcpp.hpp>

namespace runtime::actors
{
    class DisplayAO final : public QP::QActive
    {
    public:
        DisplayAO(
            application::Application &application,
            application::DisplayPort display) noexcept;

        DisplayAO(DisplayAO const &) = delete;
        DisplayAO &operator=(DisplayAO const &) = delete;

    private:
        Q_STATE_DECL(initial);
        Q_STATE_DECL(waiting);

        application::Application &application_;
        application::DisplayPort display_;
    };
} // namespace runtime::actors
