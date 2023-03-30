#pragma once

#include "EventListenerBase.hpp"

struct IEvent {
    virtual bool apply(EventListenerBase&) = 0;
    virtual ~IEvent() = default;
};
