#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "observability/logs/Emitter.hh"

// Poziom wycinany z binarki w czasie kompilacji (np. release: -DOBS_MIN_SEVERITY=9).
#ifndef OBS_MIN_SEVERITY
#define OBS_MIN_SEVERITY 1
#endif

namespace observability::logs
{
    // Odpowiednik OTel InstrumentationScope: kto emituje (np. "radio").
    struct ModuleScope
    {
        const char *name; // literał, czas życia statyczny
        std::uint8_t id;
    };

    // Lekki obiekt wartościowy, tworzony w composition root i wstrzykiwany
    // konstruktorem. Brak singletona, brak globali.
    class Logger
    {
    public:
        constexpr Logger(ModuleScope scope, LogEmitter emitter,
                         Severity runtimeMin = Severity::Info) noexcept
            : scope_{scope}, emitter_{emitter}, min_{runtimeMin} {}

        [[nodiscard]] constexpr bool enabled(Severity s) const noexcept { return s >= min_; }
        void setMinSeverity(Severity s) noexcept { min_ = s; }

        // Kopia z kontekstem trace'a: korelacja logów ze spanem (OTel trace_id/span_id).
        [[nodiscard]] constexpr Logger withTrace(const TraceContext &ctx) const noexcept
        {
            Logger copy{*this};
            copy.trace_ = ctx;
            return copy;
        }

        template <Severity S, typename... A>
            requires(LogArgType<A> && ...)
        void log(std::type_identity_t<Fmt<A...>> fmt, A... args) const noexcept
        {
            static_assert(sizeof...(A) <= kMaxArgs, "za dużo argumentów logu");
            if constexpr (static_cast<unsigned>(S) >= OBS_MIN_SEVERITY)
            {
                if (!enabled(S))
                {
                    return;
                }
                LogRecord r{};
                r.token = fmt.token;
                r.severity = S;
                r.moduleId = scope_.id;
                r.argCount = static_cast<std::uint8_t>(sizeof...(A));
                r.trace = trace_;
                [[maybe_unused]] std::size_t i = 0;
                ((r.args[i++] = makeArg(args)), ...);
                emitter_(r);
            }
        }

        template <typename... A> void trace(std::type_identity_t<Fmt<A...>> f, A... a) const noexcept { log<Severity::Trace, A...>(f, a...); }
        template <typename... A> void debug(std::type_identity_t<Fmt<A...>> f, A... a) const noexcept { log<Severity::Debug, A...>(f, a...); }
        template <typename... A> void info(std::type_identity_t<Fmt<A...>> f, A... a) const noexcept { log<Severity::Info, A...>(f, a...); }
        template <typename... A> void warn(std::type_identity_t<Fmt<A...>> f, A... a) const noexcept { log<Severity::Warn, A...>(f, a...); }
        template <typename... A> void error(std::type_identity_t<Fmt<A...>> f, A... a) const noexcept { log<Severity::Error, A...>(f, a...); }
        template <typename... A> void fatal(std::type_identity_t<Fmt<A...>> f, A... a) const noexcept { log<Severity::Fatal, A...>(f, a...); }

    private:
        ModuleScope scope_;
        LogEmitter emitter_;
        Severity min_;
        TraceContext trace_{};
    };
} // namespace observability::logs
