#pragma once
#include "actors/active_object/interface/ActiveObject.hh"

namespace actors
{
class BluetoothManager : public interface::ActiveObject
{
public:
    BluetoothManager();
    void start() override;
    void stop() override;
    void post(std::unique_ptr<interface::Event> event) override;
};
}