#include <runtime/composite_root/app/App.hh>
#include <cstdio>
#include <qpcpp.hpp>

namespace QP
{
    void QF::onStartup()
    {
        // ESP_LOGI("QP", "QF started");
    }

    void QF::onCleanup() {}
} // namespace QP

extern "C" Q_NORETURN Q_onAssert(char_t const *const module, int_t location)
{
    fprintf(stderr, "\n!!! QP ASSERTION FAILED !!!\n");
    fprintf(stderr, "Module: %s\n", module);
    fprintf(stderr, "Location: %d\n\n", location);
    fflush(stderr);
    abort();
}

namespace runtime::composite_root
{
    App::App(SystemOrchestrator &orchestrator) : orchestrator_(orchestrator) {}
    void App::run()
    {
        QP::QF::init();
        QP::QF::psInit(subscrSto_.data(), subscrSto_.size());
        QP::QF::poolInit(
            poolSto_.data(),
            poolSto_.size() * sizeof(runtime::actors::LargestEvt),
            sizeof(runtime::actors::LargestEvt));
        auto result = orchestrator_.orchestrate();
        if (!result)
        {
            std::printf(
                "[App] System orchestration failed with error %u\n",
                static_cast<unsigned>(result.error()));
        }
        // QP::QF::onStartup();
        QP::QF::run();
    }
}