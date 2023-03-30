#pragma once

#include "IEvent.hpp"

struct ButtonEvent : IEvent {
    EButton button;
    bool is_pressed;
    ButtonEvent(EButton button, bool is_pressed);
    bool apply(EventListenerBase& listener) override;
};

struct AxisRelEvent : IEvent {
    EAxisRel axis;
    float val;
    AxisRelEvent(EAxisRel axis, float val);
    bool apply(EventListenerBase& listener) override;
};

struct AxisRelEndEvent : IEvent {
    EAxisRel axis;
    AxisRelEndEvent(EAxisRel axis);
    bool apply(EventListenerBase& listener) override;
};
