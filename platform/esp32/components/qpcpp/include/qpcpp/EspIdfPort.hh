#pragma once
#include "qpcpp.hpp"

extern "C" Q_NORETURN Q_onAssert(char_t const *const module, int_t location);

using ActiveObjectPriority = std::uint_fast8_t const;
using EventQueueBuffer = QP::QEvt const **const;
using EventQueueLength = std::uint_fast16_t const;
using StackBuffer = void *const;
using StackSizeInBytes = std::uint_fast16_t const;
using InitParameter = void const *const;