#pragma once
#include <composite_root/system_orchestrator/SystemOrchestrator.hh>
#include <array>
#include <qpcpp.hpp>
#include <runtime/actors/system_ao/SystemAO.hh>

namespace runtime::composite_root
{
    class App
    {
    public:
        App(SystemOrchestrator &orchestrator);
        void run();

    private:
        SystemOrchestrator &orchestrator_;
        std::array<QP::QSubscrList, runtime::actors::Signals::MAX_SIG> subscrSto_;
        struct PoolBlock
        {
            QF_MPOOL_EL(runtime::actors::SystemAO)
            dummy;
        };
        std::array<PoolBlock, 20> poolSto_;
    };
}