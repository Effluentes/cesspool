#pragma once

#include <qpcpp.hpp>

namespace runtime::actors
{
    class DisplayAO final : public QP::QActive
    {
    public:
        DisplayAO() noexcept;

        DisplayAO(DisplayAO const &) = delete;
        DisplayAO &operator=(DisplayAO const &) = delete;

    private:
        Q_STATE_DECL(initial);
        Q_STATE_DECL(splash);
    };
} // namespace runtime::actors