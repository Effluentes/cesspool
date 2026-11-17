#include "runtime/actors/system_ao/SystemAO.hh"

#include <runtime/actors/common/Signals.hh>

namespace runtime::actors
{
    namespace
    {
        QP::QEvt const displayRefreshEvent{DISPLAY_REFRESH_SIG, 0, 0};
    }

    SystemAO::SystemAO(
        application::Application &application,
        DisplayAO &displayAO,
        configuration::interface::ConfigurationLoader configurationLoader) noexcept
        : QP::QActive(Q_STATE_CAST(&SystemAO::initial)),
          application_(application),
          displayAO_(displayAO),
          configurationLoader_(configurationLoader)
    {
    }

    void SystemAO::dispatchCommand(application::Command const &command)
    {
        const auto commandId = nextCommandId_++;
        if (nextCommandId_ == 0)
        {
            nextCommandId_ = 1;
        }
        const auto result = application_.dispatch(application::CommandEnvelope{
            commandId,
            application::CommandEnvelope::currentSchemaVersion,
            command});
        if (result.changed())
        {
            displayAO_.POST(&displayRefreshEvent, this);
        }
    }

    void SystemAO::post(application::UserInput input) noexcept
    {
        static QP::QEvt const rotateLeftEvent{CONTROL_ROTATE_LEFT_SIG, 0, 0};
        static QP::QEvt const rotateRightEvent{CONTROL_ROTATE_RIGHT_SIG, 0, 0};
        static QP::QEvt const selectEvent{CONTROL_SELECT_SIG, 0, 0};
        static QP::QEvt const confirmEvent{CONTROL_CONFIRM_SIG, 0, 0};
        static QP::QEvt const backEvent{CONTROL_BACK_SIG, 0, 0};

        switch (input)
        {
        case application::UserInput::rotateLeft:
            POST(&rotateLeftEvent, this);
            break;
        case application::UserInput::rotateRight:
            POST(&rotateRightEvent, this);
            break;
        case application::UserInput::select:
            POST(&selectEvent, this);
            break;
        case application::UserInput::confirm:
            POST(&confirmEvent, this);
            break;
        case application::UserInput::back:
            POST(&backEvent, this);
            break;
        }
    }

    Q_STATE_DEF(SystemAO, initial)
    {
        return tran(&SystemAO::active);
    }

    Q_STATE_DEF(SystemAO, active)
    {
        switch (e->sig)
        {
        case Q_ENTRY_SIG:
        {
            displayAO_.POST(&displayRefreshEvent, this);
            const auto configuration = configurationLoader_.loadConfiguration();
            auto status = application::ConfigurationStatus::failed;
            if (configuration)
            {
                status = application::ConfigurationStatus::loaded;
            }
            else if (configuration.error() ==
                     configuration::interface::ConfigurationLoaderError::CONFIGURATION_NOT_FOUND)
            {
                status = application::ConfigurationStatus::missing;
            }
            dispatchCommand(application::BootCompleted{status});
            return Q_HANDLED();
        }
        case CONTROL_ROTATE_LEFT_SIG:
            dispatchCommand(application::Rotate{application::Direction::left});
            return Q_HANDLED();
        case CONTROL_ROTATE_RIGHT_SIG:
            dispatchCommand(application::Rotate{application::Direction::right});
            return Q_HANDLED();
        case CONTROL_SELECT_SIG:
            dispatchCommand(application::Select{});
            return Q_HANDLED();
        case CONTROL_CONFIRM_SIG:
            dispatchCommand(application::Confirm{});
            return Q_HANDLED();
        case CONTROL_BACK_SIG:
            dispatchCommand(application::Back{});
            return Q_HANDLED();
        default:
            return super(&QP::QHsm::top);
        }
    }
}
