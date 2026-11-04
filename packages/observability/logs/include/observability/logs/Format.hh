#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <type_traits>

namespace observability::logs
{
    inline constexpr std::size_t kMaxArgs = 4;

    [[nodiscard]] constexpr std::uint32_t fnv1a(std::string_view s) noexcept
    {
        std::uint32_t h = 2166136261u;
        for (const char c : s)
        {
            h = (h ^ static_cast<std::uint8_t>(c)) * 16777619u;
        }
        return h;
    }

    // Styl std::format: "{}" = argument, "{{" = literalny '{'.
    [[nodiscard]] constexpr std::size_t countPlaceholders(std::string_view s) noexcept
    {
        std::size_t n = 0;
        for (std::size_t i = 0; i < s.size(); ++i)
        {
            if (s[i] != '{' || i + 1 >= s.size())
            {
                continue;
            }
            if (s[i + 1] == '{')
            {
                ++i; // escaped
            }
            else if (s[i + 1] == '}')
            {
                ++n;
                ++i;
            }
        }
        return n;
    }

    // Typy argumentów, które mieszczą się w 32 bitach (RISC-V C3 / Cortex-M).
    template <typename T>
    concept LogArgType =
        (std::is_integral_v<T> || std::is_floating_point_v<T>) && sizeof(T) <= 4;

    // Celowo niezdefiniowana: wywołanie w consteval = błąd kompilacji.
    void formatPlaceholderCountMismatch();

    // Format sprawdzany w czasie kompilacji (jak std::format_string):
    // liczba "{}" musi się zgadzać z liczbą argumentów.
    template <typename... Args>
    struct Fmt
    {
        std::string_view text;
        std::uint32_t token; // hash formatu; tekst odtwarza host (gateway) z bazy tokenów

        template <std::size_t N>
        consteval Fmt(const char (&s)[N]) // NOLINT: niejawna konwersja z literału jest zamierzona
            : text{s, N - 1}, token{fnv1a(text)}
        {
            if (countPlaceholders(text) != sizeof...(Args))
            {
                formatPlaceholderCountMismatch();
            }
        }
    };
} // namespace observability::logs
