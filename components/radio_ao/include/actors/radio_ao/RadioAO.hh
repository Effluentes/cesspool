#pragma once
#include "qpcpp.hpp"
#include "actors/radio_ao/IRadioTransport.hpp"
#include "Events.hh"

// Forward declaration
class IRadioTransport;

class RadioAO final : public QP::QActive {
public:
    explicit RadioAO(IRadioTransport& transport) noexcept;

    void start(uint_fast8_t         prio,
               QP::QEvt const**     qSto,
               uint_fast16_t        qLen,
               void*                stkSto,
               uint_fast16_t        stkSize,
               const void*          initPar = nullptr) noexcept;

private:
    IRadioTransport&  transport_;
    QP::QTimeEvt      txTimeoutTimer_;

    // kopia danych do wysłania — potrzebna w Q_ENTRY_SIG transmitting
    RadioSendEvt      pendingTx_{};

    static QP::QState initial      (RadioAO* me, QP::QEvt const* e);
    static QP::QState idle         (RadioAO* me, QP::QEvt const* e);
    static QP::QState active       (RadioAO* me, QP::QEvt const* e);  // composite
    static QP::QState listening    (RadioAO* me, QP::QEvt const* e);
    static QP::QState transmitting (RadioAO* me, QP::QEvt const* e);
};
