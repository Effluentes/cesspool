#include "composite_root/app/App.hh"
#include <cstdio>
#include <qpcpp.hpp>
namespace runtime::composite_root
{
    App::App(SystemOrchestrator &orchestrator) : orchestrator_(orchestrator) {}
    void App::run()
    {
        QP::QF::init();
        QP::QF::psInit(subscrSto_.data(), subscrSto_.size());
        QP::QF::poolInit(poolSto_.data(), sizeof(poolSto_), sizeof(runtime::actors::LargestEvt));

        auto result = orchestrator_.orchestrate();
        printf("Orchestration result: %s\n", result ? "Success" : "Failure");
        if (!result)
        {
            // log_.fatal("orchestrate failed err={}", static_cast<std::uint32_t>(result.error()));
        }

        // QP::QF::onStartup();
        QP::QF::run();
    }
}