
#include "runtime/actors/system_ao/SystemAO.hh"

namespace runtime::actors
{

    namespace
    {

        // Wartość przykładowa.
        // Czas rzeczywisty = liczba ticków / częstotliwość ticków.
        constexpr QP::QTimeEvtCtr kStartupTimeoutTicks{
            5000U};

    } // namespace

    SystemAO::SystemAO() noexcept
        : QP::QActive(Q_STATE_CAST(&SystemAO::initial)), startupTimer_(this, Signals::STARTUP_TIMEOUT_SIG)
    {
    }

    // =========================================================
    // INITIAL
    // =========================================================

    Q_STATE_DEF(SystemAO, initial)
    {
        (void)e;
        return tran(&SystemAO::starting);
    }

    // =========================================================
    // STARTING
    // =========================================================

    Q_STATE_DEF(SystemAO, starting)
    {
        switch (e->sig)
        {

        case Q_ENTRY_SIG:
            platformReady_ = false;
            radioReady_ = false;
            serverConnected_ = false;
            allSlavesAvailable_ = false;

            startupTimer_.armX(kStartupTimeoutTicks);
            return Q_HANDLED();

        case Q_EXIT_SIG:
            startupTimer_.disarm();
            return Q_HANDLED();

        case Signals::PLATFORM_READY_SIG:
            if (platformReady_)
            {
                return Q_HANDLED();
            }

            platformReady_ = true;
            break;

        case Signals::RADIO_READY_SIG:
            if (radioReady_)
            {
                return Q_HANDLED();
            }

            radioReady_ = true;
            break;

        // Aktualizujemy również stan usług, które mogą
        // zgłosić gotowość przed końcem inicjalizacji.
        case Signals::SERVER_CONNECTED_SIG:
            serverConnected_ = true;
            return Q_HANDLED();

        case Signals::SERVER_DISCONNECTED_SIG:
            serverConnected_ = false;
            return Q_HANDLED();

        case Signals::ALL_SLAVES_AVAILABLE_SIG:
            allSlavesAvailable_ = true;
            return Q_HANDLED();

        case Signals::SLAVE_UNAVAILABLE_SIG:
            allSlavesAvailable_ = false;
            return Q_HANDLED();

        case Signals::STARTUP_TIMEOUT_SIG:
            return tran(&SystemAO::fault);

        default:
            return super(&QP::QHsm::top);
        }

        if (platformReady_ && radioReady_)
        {
            return tran(&SystemAO::operational);
        }

        return Q_HANDLED();
    }

    // =========================================================
    // OPERATIONAL — SUPERSTATE
    // =========================================================

    Q_STATE_DEF(SystemAO, operational)
    {
        switch (e->sig)
        {

        case Q_INIT_SIG:
            if (serverConnected_ && allSlavesAvailable_)
            {
                return tran(&SystemAO::fully_operational);
            }

            return tran(&SystemAO::degraded);

        case Signals::SERVER_CONNECTED_SIG:
            if (serverConnected_)
            {
                return Q_HANDLED();
            }

            serverConnected_ = true;

            if (allSlavesAvailable_)
            {
                return tran(&SystemAO::fully_operational);
            }

            return Q_HANDLED();

        case Signals::ALL_SLAVES_AVAILABLE_SIG:
            if (allSlavesAvailable_)
            {
                return Q_HANDLED();
            }

            allSlavesAvailable_ = true;

            if (serverConnected_)
            {
                return tran(&SystemAO::fully_operational);
            }

            return Q_HANDLED();

        default:
            return super(&QP::QHsm::top);
        }
    }

    // =========================================================
    // FULLY OPERATIONAL
    // =========================================================

    Q_STATE_DEF(SystemAO, fully_operational)
    {
        switch (e->sig)
        {

        case Signals::SERVER_DISCONNECTED_SIG:
            if (!serverConnected_)
            {
                return Q_HANDLED();
            }

            serverConnected_ = false;
            return tran(&SystemAO::degraded);

        case Signals::SLAVE_UNAVAILABLE_SIG:
            if (!allSlavesAvailable_)
            {
                return Q_HANDLED();
            }

            allSlavesAvailable_ = false;
            return tran(&SystemAO::degraded);

        default:
            return super(&SystemAO::operational);
        }
    }

    // =========================================================
    // DEGRADED
    // =========================================================

    Q_STATE_DEF(SystemAO, degraded)
    {
        switch (e->sig)
        {

        case Signals::SERVER_DISCONNECTED_SIG:
            if (!serverConnected_)
            {
                return Q_HANDLED();
            }

            serverConnected_ = false;
            return Q_HANDLED();

        case Signals::SLAVE_UNAVAILABLE_SIG:
            if (!allSlavesAvailable_)
            {
                return Q_HANDLED();
            }

            allSlavesAvailable_ = false;
            return Q_HANDLED();

        default:
            return super(&SystemAO::operational);
        }
    }

    // =========================================================
    // FAULT
    // =========================================================

    Q_STATE_DEF(SystemAO, fault)
    {
        switch (e->sig)
        {

        case Q_ENTRY_SIG:
            // TODO:
            // 1. Zarejestruj przyczynę awarii.
            // 2. Zwiększ system.startup_timeout_total.
            // 3. Powiadom warstwę koordynującą o błędzie.
            return Q_HANDLED();

        case Signals::SYSTEM_RESTART_REQUEST_SIG:
            // Ponowna próba wyłącznie na jawne żądanie.
            return tran(&SystemAO::starting);

        case Signals::STARTUP_TIMEOUT_SIG:
            // Spóźniony lub powtórzony timeout.
            return Q_HANDLED();

        default:
            return super(&QP::QHsm::top);
        }
    }

} // namespace runtime::actors
