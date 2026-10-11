#pragma once

#include "domain/SystemState.hh"
#include "ports/PlatformTraits.hh"

#include <qpcpp.hpp>

#include <cstdint>
#include <span>
#include <type_traits>

namespace runtime::actors
{
template <Display DisplayT, BleTransport BleT, EncoderSource EncoderT>
class SystemStateAO final : public QP::QActive
{
public:
    SystemStateAO(DisplayT &display, BleT &ble, EncoderT &encoder)
        : QP::QActive(Q_STATE_CAST(&SystemStateAO::initial))
        , display_(display)
        , ble_(ble)
        , encoder_(encoder)
    {}

    cesspool::domain::SystemState const &state() const noexcept
    {
        return state_;
    }

private:
    static QP::QState initial(SystemStateAO *me, QP::QEvt const *event)
    {
        (void)event;
        me->ble_.onConnect(&SystemStateAO::onBleConnect, me);
        me->ble_.onData(&SystemStateAO::onBleData, me);
        return Q_TRAN(Q_STATE_CAST(&SystemStateAO::running));
    }

    static QP::QState running(SystemStateAO *me, QP::QEvt const *event)
    {
        switch (event->sig) {
        case Q_ENTRY_SIG:
            (void)me->display_.draw("water level monitor");
            return Q_HANDLED();

        case Q_EXIT_SIG:
            return Q_HANDLED();

        case QP::Q_USER_SIG:
            me->pollEncoder();
            return Q_HANDLED();

        default:
            return Q_SUPER(Q_STATE_CAST(&QP::QHsm::top));
        }
    }

    void pollEncoder()
    {
        auto delta = encoder_.poll();
        if (delta.has_value()) {
            auto const current = state_.waterLevel.millimetres();
            auto const adjustment = delta->value;
            auto const next = adjustment < 0 &&
                                      current < static_cast<std::uint16_t>(-adjustment)
                                  ? 0U
                                  : static_cast<unsigned int>(current) + adjustment;
            auto const bounded = next > UINT16_MAX ? UINT16_MAX : next;
            state_.waterLevel = cesspool::domain::WaterLevel{
                static_cast<std::uint16_t>(bounded)};
        }
    }

    static void onBleConnect(void *context, bool connected)
    {
        auto *me = static_cast<SystemStateAO *>(context);
        me->state_.bleConnected = connected;
    }

    static void onBleData(void *context, std::span<std::uint8_t const> payload)
    {
        (void)context;
        (void)payload;
    }

    DisplayT &display_;
    BleT &ble_;
    EncoderT &encoder_;
    cesspool::domain::SystemState state_{};
};
} // namespace runtime::actors
