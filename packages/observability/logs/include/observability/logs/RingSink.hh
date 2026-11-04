#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>

#include "observability/logs/Concepts.hh"

namespace observability::logs
{
    // Gorąca ścieżka: stempluje czas, kopiuje rekord do bufora kołowego, wraca.
    // Zero alokacji, zero I/O. Polityka przepełnienia (jawna): odrzucamy NAJNOWSZY
    // rekord i liczymy straty; drener zgłasza je jako osobny wpis.
    // Pojemność użyteczna to N-1 (jedno pole odróżnia "pełny" od "pusty").
    template <MonotonicClock Clock, CriticalSection Cs, std::size_t N>
    class RingSink
    {
        static_assert(N >= 2, "bufor musi mieć co najmniej 2 pola");

    public:
        RingSink(const Clock &clock, Cs &cs) noexcept : clock_{clock}, cs_{cs} {}

        void write(const LogRecord &r) noexcept
        {
            const std::uint64_t ts = clock_.nowUs(); // poza sekcją krytyczną
            const std::uint32_t key = cs_.enter();
            const std::size_t next = (head_ + 1) % N;
            if (next == tail_)
            {
                ++dropped_;
            }
            else
            {
                buf_[head_] = r;
                buf_[head_].timestampUs = ts;
                head_ = next;
            }
            cs_.exit(key);
        }

        [[nodiscard]] std::optional<LogRecord> pop() noexcept
        {
            std::optional<LogRecord> out;
            const std::uint32_t key = cs_.enter();
            if (tail_ != head_)
            {
                out = buf_[tail_];
                tail_ = (tail_ + 1) % N;
            }
            cs_.exit(key);
            return out;
        }

        [[nodiscard]] std::uint32_t takeDropped() noexcept
        {
            const std::uint32_t key = cs_.enter();
            const std::uint32_t d = dropped_;
            dropped_ = 0;
            cs_.exit(key);
            return d;
        }

        // Ograniczona praca na jedno wywołanie (nie zagładza innych zadań).
        template <typename Consumer>
        std::size_t drain(Consumer &&consume, std::size_t maxRecords) noexcept
        {
            std::size_t n = 0;
            while (n < maxRecords)
            {
                auto rec = pop();
                if (!rec)
                {
                    break;
                }
                consume(*rec);
                ++n;
            }
            return n;
        }

    private:
        const Clock &clock_;
        Cs &cs_;
        std::array<LogRecord, N> buf_{};
        std::size_t head_{0};
        std::size_t tail_{0};
        std::uint32_t dropped_{0};
    };
} // namespace observability::logs
