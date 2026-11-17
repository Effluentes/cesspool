#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <span>
#include <variant>

namespace runtime::application {
    enum class Page : std::uint8_t {
        splash,
        firstConfiguration,
        mainMenu,
        slaves,
        settingsMenu,
        settingDetails,
        operational,
        error
    };

    enum class Direction : std::uint8_t { left, right };

    enum class ConfigurationStatus : std::uint8_t { loaded, missing, failed };

    struct View {
        Page page{Page::splash};
        std::uint8_t selectedIndex{0};
        friend bool operator==(View const &, View const &) = default;
    };

    struct GetCurrentView {};

    using CommandId = std::uint32_t;

    struct BootCompleted {
        ConfigurationStatus configurationStatus;
    };

    struct Rotate {
        Direction direction;
    };

    struct Select {};

    struct Confirm {};

    struct Back {};

    using Command = std::variant<BootCompleted, Rotate, Select, Confirm, Back>;

    struct CommandEnvelope {
        static constexpr std::uint16_t currentSchemaVersion = 1;

        CommandId id{0};
        std::uint16_t schemaVersion{currentSchemaVersion};
        Command payload;
    };

    struct EventMetadata {
        static constexpr std::uint16_t currentSchemaVersion = 1;

        CommandId commandId{0};
        std::uint16_t schemaVersion{currentSchemaVersion};
    };

    struct PageChanged {
        EventMetadata metadata{};
        View from{};
        View to{};
    };

    struct SelectionChanged {
        EventMetadata metadata{};
        Page page{Page::splash};
        std::uint8_t from{0};
        std::uint8_t to{0};
    };

    using DomainEvent = std::variant<PageChanged, SelectionChanged>;

    enum class CommandDisposition : std::uint8_t { accepted, duplicate, unsupportedVersion };

    struct CommandResult {
        std::array<DomainEvent, 1> events{};
        std::size_t eventCount{0};
        CommandDisposition disposition{CommandDisposition::accepted};

        [[nodiscard]] bool changed() const noexcept {
            return eventCount != 0;
        }
    };

    class Application final {
      public:
        [[nodiscard]] CommandResult dispatch(Command const &command) noexcept;
        [[nodiscard]] CommandResult dispatch(CommandEnvelope const &command) noexcept;
        [[nodiscard]] View query(GetCurrentView) const noexcept;

      private:
        [[nodiscard]] CommandResult completeBoot(BootCompleted const &command,
                                                 EventMetadata metadata) noexcept;
        [[nodiscard]] CommandResult rotate(Rotate const &command, EventMetadata metadata) noexcept;
        [[nodiscard]] CommandResult select(EventMetadata metadata) noexcept;
        [[nodiscard]] CommandResult back(EventMetadata metadata) noexcept;
        [[nodiscard]] CommandResult changePage(Page page, std::uint8_t selectedIndex,
                                               EventMetadata metadata) noexcept;
        [[nodiscard]] bool hasProcessed(CommandId commandId) const noexcept;
        void remember(CommandId commandId) noexcept;
        [[nodiscard]] static constexpr std::uint32_t pack(View view) noexcept {
            return static_cast<std::uint32_t>(view.page) |
                   (static_cast<std::uint32_t>(view.selectedIndex) << 8U);
        }
        [[nodiscard]] View currentView() const noexcept;

        std::atomic<std::uint32_t> viewState_{0};
        std::uint8_t mainMenuSelection_{0};
        std::uint8_t settingsSelection_{0};
        std::array<CommandId, 16> processedCommandIds_{};
        std::size_t nextProcessedCommandId_{0};
    };

    using StatusView = View;

    [[nodiscard]] StatusView rebuildStatusView(StatusView initial,
                                               std::span<DomainEvent const> events) noexcept;
} // namespace runtime::application
