#pragma once

#include <qpcpp.hpp>

namespace runtime::actors
{
    class WiFiAO final : public QP::QActive
    {
    public:
        WiFiAO() noexcept;

        WiFiAO(WiFiAO const &) = delete;
        WiFiAO &operator=(WiFiAO const &) = delete;

    private:
        Q_STATE_DECL(initial);
        Q_STATE_DECL(idle);
    };
}
