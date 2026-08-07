#pragma once
#include <qpcpp.hpp>
#include <qpcpp/EspIdfPort.hh>
#include <qpcpp/Signals.hh>

struct SystemStateSnapshotEvt : QP::QEvt
{
};

class SystemStateAO final : public QP::QActive
{
public:
    SystemStateAO();
    void start(
        ActiveObjectPriority prio,
        EventQueueBuffer qSto,
        EventQueueLength qLen,
        StackBuffer stkSto,
        StackSizeInBytes stkSize,
        InitParameter par = nullptr) override;

private:
    void publishSnapshot();

    static QP::QState initial(SystemStateAO *me, QP::QEvt const *e);
    static QP::QState running(SystemStateAO *me, QP::QEvt const *e);

    QP::QEvt const *queue_[16];
};