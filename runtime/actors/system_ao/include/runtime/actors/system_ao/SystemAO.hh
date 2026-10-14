#pragma once
#include <qpcpp.hpp>

namespace runtime::actors
{
    // Zmieniono z QP::Active na QP::QActive
    class SystemAO final : public QP::QActive
    {
    public:
        SystemAO() noexcept;

    protected:
        Q_STATE_DECL(initial);
        Q_STATE_DECL(starting);
        Q_STATE_DECL(operational);
        Q_STATE_DECL(fault);
    };
}