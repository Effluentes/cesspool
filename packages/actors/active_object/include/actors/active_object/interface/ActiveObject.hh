#pragma once
#include <memory>
namespace actors::interface
{
struct Event;
struct ActiveObject
{
    virtual ~ActiveObject() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
virtual void post(std::unique_ptr<Event> event) = 0;
};
}