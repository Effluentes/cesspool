#pragma once
#include <qpcpp.hpp>

enum class EncoderNavDirection : std::uint8_t
{
    None,
    Left,
    Right,
    Click
};

struct EncoderEvt : public QP::QEvt
{
    EncoderNavDirection navDirection{EncoderNavDirection::None};
};