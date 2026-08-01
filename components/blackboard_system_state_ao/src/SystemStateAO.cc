#include "blackboard_system_state_ao/SystemStateAO.hh"
SystemStateAO::SystemStateAO() : QP::QActive(Q_STATE_CAST(&initial))
{
}

void SystemStateAO::start(
    ActiveObjectPriority prio,
    EventQueueBuffer qSto,
    EventQueueLength qLen,
    StackBuffer stkSto,
    StackSizeInBytes stkSize,
    InitParameter par)
{
    QActive::start(prio, qSto, qLen, stkSto, stkSize, par);
}

void SystemStateAO::publishSnapshot()
{
    auto *evt = Q_NEW(SystemStateSnapshotEvt, SYSTEM_SNAPSHOT_SIG);
    QP::QF::PUBLISH(evt, this);
}

QP::QState SystemStateAO::initial(SystemStateAO *const me,
                                  QP::QEvt const *const e)
{
    (void)e;
    me->subscribe(SENSOR_DATA_SIG);
    me->subscribe(BT_CONNECTED_SIG);
    me->subscribe(BT_DISCONNECTED_SIG);
    me->subscribe(BT_DATA_SIG);
    return Q_TRAN(Q_STATE_CAST(&running));
}

QP::QState SystemStateAO::running(SystemStateAO *const me,
                                  QP::QEvt const *const e)
{
    switch (e->sig)
    {
    case Q_ENTRY_SIG:
        return Q_HANDLED();

    case Q_EXIT_SIG:
        return Q_HANDLED();

    case SENSOR_DATA_SIG:
    {
        return Q_HANDLED();
    }

    case BT_CONNECTED_SIG:
        return Q_HANDLED();

    case BT_DISCONNECTED_SIG:
        return Q_HANDLED();

    default:
        return Q_SUPER(Q_STATE_CAST(&QP::QHsm::top));
    }
}