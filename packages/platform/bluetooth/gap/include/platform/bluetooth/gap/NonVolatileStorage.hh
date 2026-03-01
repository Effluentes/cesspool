#pragma once
#include "platform/storage/storage/interface/Storage.hh"

namespace platform::storage
{
class NonVolatileStorage : public interface::Storage
{
public:
    NonVolatileStorage();
    ~NonVolatileStorage() override;
    bool initialize() override;
private:
    bool erase();
};
}