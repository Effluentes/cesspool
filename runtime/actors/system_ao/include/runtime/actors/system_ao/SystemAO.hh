#pragma once

#include <cstdint>
#include <qpcpp.hpp>
#include <runtime/actors/common/Signals.hh>
#include <runtime/actors/display_ao/DisplayAO.hh>
#include <configuration/configuration/interface/ConfigurationLoader.hh>

namespace runtime::actors
{
    class SystemAO final : public QP::QActive
    {
    public:
        SystemAO(
            DisplayAO &displayAO,
            configuration::interface::ConfigurationLoader &configurationLoader) noexcept;

        SystemAO(SystemAO const &) = delete;
        SystemAO &operator=(SystemAO const &) = delete;

    private:
        Q_STATE_DECL(initial);
        Q_STATE_DECL(boot);
        Q_STATE_DECL(firstConfiguration);
        Q_STATE_DECL(menu);
        Q_STATE_DECL(slaves);
        Q_STATE_DECL(settings);
        Q_STATE_DECL(settingDetail);
        Q_STATE_DECL(operational);

        DisplayAO &displayAO_;
        configuration::interface::ConfigurationLoader &configurationLoader_;
        QP::QTimeEvt startupTimer_;
        std::uint8_t selectedMenuItem_{0};
        std::uint8_t selectedSetting_{0};
        bool configurationMissing_{false};
        bool configurationLoaded_{false};
    };
    struct LargestEvt : public QP::QEvt
    {
        LargestEvt() : QP::QEvt{static_cast<QP::QSignal>(MAX_SIG), 0, 0} {}
    };

} // namespace runtime::actors
