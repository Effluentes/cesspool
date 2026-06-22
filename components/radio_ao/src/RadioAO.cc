#include "actors/radio_ao/RadioAO.hh"
// #include "bsp.hpp"   // BSP_TICKS_PER_SEC

Q_DEFINE_THIS_FILE

// Define BSP tick rate if not defined elsewhere (ticks per second)
#ifndef BSP_TICKS_PER_SEC
    #define BSP_TICKS_PER_SEC 100U
#endif

// ──────────────────────────────────────────────
// Constructor
// ──────────────────────────────────────────────
RadioAO::RadioAO(IRadioTransport& transport) noexcept
    : QP::QActive(Q_STATE_CAST(&initial))
    , transport_(transport)
    , txTimeoutTimer_(this, RADIO_TX_TIMEOUT_SIG, 0U)  // tick rate 0
{}

void RadioAO::start(uint_fast8_t     prio,
                    QP::QEvt const** qSto,
                    uint_fast16_t    qLen,
                    void*            stkSto,
                    uint_fast16_t    stkSize,
                    const void*      initPar) noexcept
{
    QActive::start(prio, qSto, qLen, stkSto, stkSize, initPar);
}

// ──────────────────────────────────────────────
// Pseudostate initial
// ──────────────────────────────────────────────
QP::QState RadioAO::initial(RadioAO* const me, QP::QEvt const* const e) {
    (void)e;
    me->subscribe(RADIO_INIT_SIG);
    me->subscribe(RADIO_SEND_SIG);
    // RADIO_DATA_RX_SIG pochodzi z mostu transportowego (POST, nie PUBLISH)
    return Q_TRAN(Q_STATE_CAST(&idle));
}

// ──────────────────────────────────────────────
// State: idle
// czeka na inicjalizację transportu
// ──────────────────────────────────────────────
QP::QState RadioAO::idle(RadioAO* const me, QP::QEvt const* const e) {
    QP::QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            status = Q_HANDLED();
            break;
        }
        case RADIO_INIT_SIG: {
            me->transport_.init();
            status = Q_TRAN(Q_STATE_CAST(&active));
            break;
        }
        default: {
            status = Q_SUPER(Q_STATE_CAST(&initial));
            break;
        }
    }
    return status;
}

// ──────────────────────────────────────────────
// State: active  (composite — domyślny substate: listening)
// ──────────────────────────────────────────────
QP::QState RadioAO::active(RadioAO* const me, QP::QEvt const* const e) {
    QP::QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            status = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG: {
            // wymagane dla composite state w QP v6.x
            status = Q_TRAN(Q_STATE_CAST(&listening));
            break;
        }
        case Q_EXIT_SIG: {
            me->txTimeoutTimer_.disarm();
            status = Q_HANDLED();
            break;
        }
        default: {
            status = Q_SUPER(Q_STATE_CAST(&idle));
            break;
        }
    }
    return status;
}

// ──────────────────────────────────────────────
// Sub-state: listening
// nasłuchuje danych od slave'ów
// ──────────────────────────────────────────────
QP::QState RadioAO::listening(RadioAO* const me, QP::QEvt const* const e) {
    QP::QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            me->transport_.startReceive();
            status = Q_HANDLED();
            break;
        }
        case RADIO_DATA_RX_SIG: {
            auto const* rx = Q_EVT_CAST(RadioDataEvt);

            // Transformuj i publikuj na event bus → TelemetryAO + DisplayManagerAO
            auto* pub = Q_NEW(SensorDataEvt, SENSOR_DATA_SIG);
            pub->slaveId    = rx->slaveId;
            pub->sensorType = rx->sensorType;
            pub->value      = rx->value;
            QP::QF::PUBLISH(pub, me);

            status = Q_HANDLED();
            break;
        }
        case RADIO_SEND_SIG: {
            // Zapamiętaj dane — Q_ENTRY_SIG transmitting ich nie dostanie
            me->pendingTx_ = *Q_EVT_CAST(RadioSendEvt);
            status = Q_TRAN(Q_STATE_CAST(&transmitting));
            break;
        }
        default: {
            status = Q_SUPER(Q_STATE_CAST(&active));
            break;
        }
    }
    return status;
}

// ──────────────────────────────────────────────
// Sub-state: transmitting
// wysyła komendę do slave'a, czeka na timeout
// ──────────────────────────────────────────────
QP::QState RadioAO::transmitting(RadioAO* const me, QP::QEvt const* const e) {
    QP::QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            me->transport_.send(me->pendingTx_.slaveId,
                                me->pendingTx_.payload,
                                me->pendingTx_.len);
            // one-shot 500 ms
            // me->txTimeoutTimer_.armX(0U, BSP_TICKS_PER_SEC / 2U, 0U);
            me->txTimeoutTimer_.armX(BSP_TICKS_PER_SEC / 2U);
            status = Q_HANDLED();
            break;
        }
        case RADIO_TX_TIMEOUT_SIG: {
            // Brak ACK — wróć do nasłuchu
            status = Q_TRAN(Q_STATE_CAST(&listening));
            break;
        }
        case Q_EXIT_SIG: {
            me->txTimeoutTimer_.disarm();
            status = Q_HANDLED();
            break;
        }
        default: {
            status = Q_SUPER(Q_STATE_CAST(&active));
            break;
        }
    }
    return status;
}
