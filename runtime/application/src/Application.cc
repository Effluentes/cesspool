#include "runtime/application/Application.hh"

#include <atomic>
#include <cstdint>
#include <span>
#include <type_traits>
#include <variant>

namespace runtime::application {
    namespace {
        template <typename> inline constexpr bool unhandledCommand = false;
    }

    CommandResult Application::dispatch(Command const &command) noexcept {
        return dispatch(CommandEnvelope{0, CommandEnvelope::currentSchemaVersion, command});
    }

    CommandResult Application::dispatch(CommandEnvelope const &envelope) noexcept {
        if (envelope.schemaVersion != CommandEnvelope::currentSchemaVersion) {
            CommandResult result;
            result.disposition = CommandDisposition::unsupportedVersion;
            return result;
        }
        if (envelope.id != 0 && hasProcessed(envelope.id)) {
            CommandResult result;
            result.disposition = CommandDisposition::duplicate;
            return result;
        }
        remember(envelope.id);

        const EventMetadata metadata{envelope.id, EventMetadata::currentSchemaVersion};
        return std::visit(
            [this, metadata](auto const &value) -> CommandResult {
                using CommandType = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<CommandType, BootCompleted>) {
                    return completeBoot(value, metadata);
                } else if constexpr (std::is_same_v<CommandType, Rotate>) {
                    return rotate(value, metadata);
                } else if constexpr (std::is_same_v<CommandType, Select> ||
                                     std::is_same_v<CommandType, Confirm>) {
                    return select(metadata);
                } else if constexpr (std::is_same_v<CommandType, Back>) {
                    return back(metadata);
                } else {
                    static_assert(unhandledCommand<CommandType>);
                }
            },
            envelope.payload);
    }

    View Application::query(GetCurrentView) const noexcept {
        return currentView();
    }

    View Application::currentView() const noexcept {
        const auto packed = viewState_.load(std::memory_order_acquire);
        return View{static_cast<Page>(packed & 0xFFU),
                    static_cast<std::uint8_t>((packed >> 8U) & 0xFFU)};
    }

    CommandResult Application::completeBoot(BootCompleted const &command,
                                            EventMetadata metadata) noexcept {
        switch (command.configurationStatus) {
        case ConfigurationStatus::loaded:
            return changePage(Page::operational, 0, metadata);
        case ConfigurationStatus::missing:
            return changePage(Page::firstConfiguration, 0, metadata);
        case ConfigurationStatus::failed:
            return changePage(Page::error, 0, metadata);
        }
        return {};
    }

    CommandResult Application::rotate(Rotate const &command, EventMetadata metadata) noexcept {
        auto view = currentView();
        std::uint8_t itemCount = 0;
        switch (view.page) {
        case Page::mainMenu:
            itemCount = 2;
            break;
        case Page::settingsMenu:
            itemCount = 3;
            break;
        default:
            return {};
        }

        const auto previous = view.selectedIndex;
        if (command.direction == Direction::right) {
            view.selectedIndex = static_cast<std::uint8_t>((previous + 1U) % itemCount);
        } else {
            view.selectedIndex = static_cast<std::uint8_t>((previous + itemCount - 1U) % itemCount);
        }

        if (view.selectedIndex == previous) {
            return {};
        }

        if (view.page == Page::mainMenu) {
            mainMenuSelection_ = view.selectedIndex;
        } else {
            settingsSelection_ = view.selectedIndex;
        }

        viewState_.store(pack(view), std::memory_order_release);
        CommandResult result;
        result.events[0] = SelectionChanged{metadata, view.page, previous, view.selectedIndex};
        result.eventCount = 1;
        return result;
    }

    CommandResult Application::select(EventMetadata metadata) noexcept {
        const auto view = currentView();
        switch (view.page) {
        case Page::firstConfiguration:
            return changePage(Page::mainMenu, mainMenuSelection_, metadata);
        case Page::mainMenu:
            if (view.selectedIndex == 0) {
                return changePage(Page::slaves, 0, metadata);
            }
            return changePage(Page::settingsMenu, settingsSelection_, metadata);
        case Page::settingsMenu:
            return changePage(Page::settingDetails, view.selectedIndex, metadata);
        default:
            return {};
        }
    }

    CommandResult Application::back(EventMetadata metadata) noexcept {
        switch (currentView().page) {
        case Page::slaves:
        case Page::settingsMenu:
            return changePage(Page::mainMenu, mainMenuSelection_, metadata);
        case Page::settingDetails:
            return changePage(Page::settingsMenu, settingsSelection_, metadata);
        case Page::mainMenu:
            return changePage(Page::firstConfiguration, 0, metadata);
        default:
            return {};
        }
    }

    CommandResult Application::changePage(Page page, std::uint8_t selectedIndex,
                                          EventMetadata metadata) noexcept {
        const auto previous = currentView();
        const View next{page, selectedIndex};
        if (previous.page == page && previous.selectedIndex == selectedIndex) {
            return {};
        }

        viewState_.store(pack(next), std::memory_order_release);
        CommandResult result;
        result.events[0] = PageChanged{metadata, previous, next};
        result.eventCount = 1;
        return result;
    }

    bool Application::hasProcessed(CommandId commandId) const noexcept {
        for (const auto processed : processedCommandIds_) {
            if (processed == commandId) {
                return true;
            }
        }
        return false;
    }

    void Application::remember(CommandId commandId) noexcept {
        if (commandId == 0) {
            return;
        }
        processedCommandIds_[nextProcessedCommandId_] = commandId;
        nextProcessedCommandId_ = (nextProcessedCommandId_ + 1U) % processedCommandIds_.size();
    }

    StatusView rebuildStatusView(StatusView initial, std::span<DomainEvent const> events) noexcept {
        for (auto const &event : events) {
            std::visit(
                [&initial](auto const &value) {
                    using EventType = std::decay_t<decltype(value)>;
                    if constexpr (std::is_same_v<EventType, PageChanged>) {
                        initial = value.to;
                    } else {
                        if (initial.page == value.page) {
                            initial.selectedIndex = value.to;
                        }
                    }
                },
                event);
        }
        return initial;
    }
} // namespace runtime::application
