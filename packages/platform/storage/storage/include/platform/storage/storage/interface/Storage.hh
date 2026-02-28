#pragma once

namespace platform::storage::interface
{
class Storage
{
public:
    virtual ~Storage() = default;
    virtual bool initialize() = 0;
};
}
