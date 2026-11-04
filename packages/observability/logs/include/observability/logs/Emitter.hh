#pragma once

#include "observability/logs/Concepts.hh"

namespace observability::logs
{
    // Wymazanie typu bez virtuali i bez sterty: wskaźnik na funkcję + kontekst.
    // Moduły w packages/ widzą tylko ten typ, nie znają konkretnego sinka
    // (nie muszą być szablonami).
    class LogEmitter
    {
    public:
        using Fn = void (*)(void *, const LogRecord &) noexcept;

        constexpr LogEmitter() noexcept = default; // pusty emitter odrzuca rekordy
        constexpr LogEmitter(Fn fn, void *ctx) noexcept : fn_{fn}, ctx_{ctx} {}

        template <LogSink S>
        [[nodiscard]] static LogEmitter bind(S &sink) noexcept
        {
            return LogEmitter{
                [](void *c, const LogRecord &r) noexcept { static_cast<S *>(c)->write(r); },
                &sink};
        }

        void operator()(const LogRecord &r) const noexcept
        {
            if (fn_ != nullptr)
            {
                fn_(ctx_, r);
            }
        }

    private:
        Fn fn_{nullptr};
        void *ctx_{nullptr};
    };
} // namespace observability::logs
