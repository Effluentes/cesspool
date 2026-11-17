#include <runtime/composite_root/app/App.hh>
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
    std::expected<void, SystemOrchestratorErrorStatus> App::run()
    {
        QP::QF::init();
        const auto result = orchestrator_.orchestrate();
        if (!result)
        {
            return std::unexpected(result.error());
        }
        QP::QF::run();
        return {};
    }
}
