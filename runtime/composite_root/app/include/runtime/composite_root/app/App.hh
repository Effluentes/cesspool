#pragma once
#include <runtime/composite_root/system_orchestrator/SystemOrchestrator.hh>
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
        std::array<QP::QSubscrList, runtime::actors::Events::MAX_SIG> subscrSto_;
        alignas(void *) std::array<runtime::actors::LargestEvt, 20> poolSto_;
    };
}