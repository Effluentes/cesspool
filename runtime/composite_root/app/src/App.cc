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
    // 1. Używamy stderr i robimy fflush, aby tekst NATYCHMIAST pojawił się na ekranie
    fprintf(stderr, "\n!!! QP ASSERTION FAILED !!!\n");
    fprintf(stderr, "Module: %s\n", module);
    fprintf(stderr, "Location: %d\n\n", location);
    fflush(stderr);

    // 2. Wywołujemy systemowy abort(). Zamiast Watchdoga, ESP-IDF zrobi ładny zrzut pamięci (panic dump)
    abort();
}

namespace runtime::composite_root
{
    App::App(SystemOrchestrator &orchestrator) : orchestrator_(orchestrator) {}
    void App::run()
    {
        printf("Orchestration\n");

        QP::QF::init();
        printf("Orchestration\n");

        QP::QF::psInit(subscrSto_.data(), subscrSto_.size());
        printf("Orchestration\n");
        printf("sizeof(LargestEvt): %zu\n", sizeof(runtime::actors::LargestEvt));
        QP::QF::poolInit(
            poolSto_.data(),                                       // Wskaźnik na poprawnie wyrównany bufor
            poolSto_.size() * sizeof(runtime::actors::LargestEvt), // Całkowity rozmiar w bajtach
            sizeof(runtime::actors::LargestEvt)                    // Rozmiar pojedynczego bloku zdarzenia
        );
        printf("Orchestration\n");

        auto result = orchestrator_.orchestrate();
        printf("Orchestration result: %s\n", result ? "Success" : "Failure");
        if (!result)
        {
            // log_.fatal("orchestrate failed err={}", static_cast<std::uint32_t>(result.error()));
        }
        printf("Orchestration\n");

        // QP::QF::onStartup();
        QP::QF::run();
        printf("Orchestration\n");
    }
}