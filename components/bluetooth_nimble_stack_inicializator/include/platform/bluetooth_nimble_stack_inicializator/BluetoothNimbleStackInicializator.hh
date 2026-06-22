#pragma once
#include "platform/bsp/startup_result/interface/Initializable.hh"

namespace platform
{
    class BluetoothNimbleStackInicializator : public platform::bsp::interface::Initializable
    {
    public:
        BluetoothNimbleStackInicializator();
        void initialize();
        bool isInitialized() const;
};
}