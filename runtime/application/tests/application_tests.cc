#include <runtime/application/Application.hh>

#include <catch2/catch_test_macros.hpp>

#include <cstdlib>
#include <iostream>
#include <string_view>

using namespace runtime::application;

namespace {
    View query(Application const &application) {
        return application.query(GetCurrentView{});
    }

    void expect(bool condition, std::string_view message) {
        INFO(message);
        REQUIRE(condition);
    }

    void send(Application &application, Command const &command) {
        static_cast<void>(application.dispatch(command));
    }

    void bootMissingConfiguration(Application &application) {
        const auto result = application.dispatch(BootCompleted{ConfigurationStatus::missing});
        expect(result.changed(), "missing configuration changes page");
        const auto *pageChanged = std::get_if<PageChanged>(&result.events[0]);
        expect(pageChanged != nullptr, "boot completion publishes PageChanged");
        expect(pageChanged->to.page == Page::firstConfiguration,
               "PageChanged identifies first-configuration destination");
        expect(query(application).page == Page::firstConfiguration,
               "missing configuration opens first-configuration page");
    }

    void testBootRouting() {
        Application missing;
        bootMissingConfiguration(missing);

        Application loaded;
        send(loaded, BootCompleted{ConfigurationStatus::loaded});
        expect(query(loaded).page == Page::operational,
               "loaded configuration opens operational page");

        Application failed;
        send(failed, BootCompleted{ConfigurationStatus::failed});
        expect(query(failed).page == Page::error, "configuration failure opens error page");
    }

    void testMainMenuNavigationWrapsBothDirections() {
        Application application;
        bootMissingConfiguration(application);
        send(application, Confirm{});
        expect(query(application).page == Page::mainMenu, "confirm opens main menu");
        expect(query(application).selectedIndex == 0, "main menu starts on Slaves");

        const auto leftResult = application.dispatch(Rotate{Direction::left});
        expect(query(application).selectedIndex == 1, "left wraps to last main-menu item");
        expect(leftResult.eventCount == 1, "rotation publishes one domain event");
        const auto *selectionChanged = std::get_if<SelectionChanged>(&leftResult.events[0]);
        expect(selectionChanged != nullptr, "rotation publishes SelectionChanged");
        expect(selectionChanged->metadata.schemaVersion == EventMetadata::currentSchemaVersion,
               "domain event has the current schema version");
        expect(selectionChanged->to == 1, "event contains selected menu index");

        const auto rightResult = application.dispatch(Rotate{Direction::right});
        expect(query(application).selectedIndex == 0, "right wraps to first main-menu item");
        expect(std::get_if<SelectionChanged>(&rightResult.events[0]) != nullptr,
               "right rotation publishes SelectionChanged");
    }

    void testSlavesNavigation() {
        Application application;
        bootMissingConfiguration(application);
        send(application, Select{});
        send(application, Select{});
        expect(query(application).page == Page::slaves, "selecting Slaves opens list");

        send(application, Back{});
        expect(query(application).page == Page::mainMenu, "back from Slaves returns to menu");
        expect(query(application).selectedIndex == 0, "main-menu selection is preserved");
    }

    void testSettingsNavigationAndSelection() {
        Application application;
        bootMissingConfiguration(application);
        send(application, Confirm{});
        send(application, Rotate{Direction::right});
        send(application, Confirm{});
        expect(query(application).page == Page::settingsMenu, "Settings opens settings menu");
        expect(query(application).selectedIndex == 0, "settings menu starts at first item");

        send(application, Rotate{Direction::left});
        expect(query(application).selectedIndex == 2,
               "settings menu wraps left across three items");
        send(application, Rotate{Direction::right});
        expect(query(application).selectedIndex == 0,
               "settings menu wraps right across three items");

        send(application, Rotate{Direction::right});
        send(application, Select{});
        expect(query(application).page == Page::settingDetails,
               "select opens selected setting details");
        expect(query(application).selectedIndex == 1, "selected Display setting is retained");

        send(application, Back{});
        expect(query(application).page == Page::settingsMenu, "back returns to settings menu");
        expect(query(application).selectedIndex == 1, "settings selection is preserved");
    }

    void testCommandsOutsideMenusAreNoOps() {
        Application application;
        const auto result = application.dispatch(Rotate{Direction::left});
        expect(!result.changed(), "rotation outside a menu emits no event");
        expect(query(application).page == Page::splash, "no-op leaves current page unchanged");
    }

    void testCommandIdsAreIdempotentAndVersionsAreChecked() {
        Application application;
        bootMissingConfiguration(application);

        const CommandEnvelope command{41, CommandEnvelope::currentSchemaVersion, Confirm{}};
        const auto firstResult = application.dispatch(command);
        expect(firstResult.changed(), "first command with id is applied");
        const auto *commandEvent = std::get_if<PageChanged>(&firstResult.events[0]);
        expect(commandEvent != nullptr, "command publishes a page-change fact");
        expect(commandEvent->metadata.commandId == command.id,
               "fact carries the originating command id");
        const auto duplicateResult = application.dispatch(command);
        expect(duplicateResult.disposition == CommandDisposition::duplicate,
               "repeated command id is rejected as a duplicate");
        expect(!duplicateResult.changed(), "duplicate command emits no event");
        expect(query(application).page == Page::mainMenu,
               "duplicate command does not apply its transition twice");

        const CommandEnvelope unsupported{42, 99, Back{}};
        const auto unsupportedResult = application.dispatch(unsupported);
        expect(unsupportedResult.disposition == CommandDisposition::unsupportedVersion,
               "unsupported command schema version is rejected");
        expect(query(application).page == Page::mainMenu,
               "unsupported command does not change application state");

        const CommandEnvelope supportedRetry{unsupported.id, CommandEnvelope::currentSchemaVersion,
                                             Back{}};
        const auto retryResult = application.dispatch(supportedRetry);
        expect(retryResult.disposition == CommandDisposition::accepted,
               "unsupported commands do not reserve their command id");
        expect(query(application).page == Page::firstConfiguration,
               "a supported retry with the same id is processed");
    }

    void testStatusViewCanBeRebuiltFromDomainEvents() {
        Application application;
        std::array<DomainEvent, 4> history{};
        std::size_t historySize = 0;
        const auto record = [&history, &historySize](CommandResult const &result) {
            if (result.changed()) {
                history[historySize++] = result.events[0];
            }
        };

        record(application.dispatch(BootCompleted{ConfigurationStatus::missing}));
        record(application.dispatch(Confirm{}));
        record(application.dispatch(Rotate{Direction::right}));
        record(application.dispatch(Confirm{}));

        const auto rebuilt =
            rebuildStatusView(View{}, std::span<DomainEvent const>{history.data(), historySize});
        expect(rebuilt == query(application),
               "replaying page and selection events rebuilds the current status view");
    }
} // namespace

TEST_CASE("boot routes based on configuration") {
    testBootRouting();
}

TEST_CASE("main menu navigation wraps in both directions") {
    testMainMenuNavigationWrapsBothDirections();
}

TEST_CASE("slaves navigation returns to the main menu") {
    testSlavesNavigation();
}

TEST_CASE("settings navigation preserves selection") {
    testSettingsNavigationAndSelection();
}

TEST_CASE("commands outside menus are no-ops") {
    testCommandsOutsideMenusAreNoOps();
}

TEST_CASE("command ids and schema versions are enforced") {
    testCommandIdsAreIdempotentAndVersionsAreChecked();
}

TEST_CASE("status view is rebuilt from the domain event list") {
    testStatusViewCanBeRebuiltFromDomainEvents();
}
