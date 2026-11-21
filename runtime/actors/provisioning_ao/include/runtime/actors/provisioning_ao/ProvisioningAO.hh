#pragma once
#include <qpcpp.h>
namespace runtime::actors
{
    class ProvisioningAO : public QP::QActive
    {
    public:
        explicit ProvisioningAO() noexcept;

    private:
        Q_STATE_DECL(initial);
        Q_STATE_DECL(provisioning);
    };
} // namespace runtime::actors