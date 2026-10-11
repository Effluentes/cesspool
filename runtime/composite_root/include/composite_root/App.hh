#pragma once

#include "ports/PlatformTraits.hh"
#include "runtime/actors/SystemStateAO.hh"

#include <qpcpp.hpp>

#include <array>
#include <cstdint>

template <PlatformTraits Traits>
class App
{
public:
    App()
        : stateAO_(display_, ble_, encoder_)
    {}

    void run()
    {
        static QP::QSubscrList subscriptions[16];
        QP::QF::psInit(subscriptions, 16U);

        stateAO_.start(
            1U,
            queue_.data(),
            static_cast<std::uint_fast16_t>(queue_.size()),
            stack_.data(),
            static_cast<std::uint_fast16_t>(stack_.size()),
            nullptr);

        QP::QF::run();
    }

private:
    typename Traits::Display display_{};
    typename Traits::Ble ble_{};
    typename Traits::Encoder encoder_{};
    runtime::actors::SystemStateAO<
        typename Traits::Display,
        typename Traits::Ble,
        typename Traits::Encoder>
        stateAO_;
    std::array<QP::QEvt const *, 16> queue_{};
    std::array<std::uint8_t, 2048> stack_{};
};
