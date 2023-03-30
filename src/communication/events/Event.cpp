#include <communications/events/Event.hpp>

ButtonEvent::ButtonEvent(EButton button, bool is_pressed) : button(button), is_pressed(is_pressed) {}

bool ButtonEvent::apply(EventListenerBase &listener) {
    return listener.on_button_event(button, is_pressed);
}

AxisRelEvent::AxisRelEvent(EAxisRel axis, float val) : axis(axis), val(val) {}

bool AxisRelEvent::apply(EventListenerBase &listener) {
    return listener.on_rel_axis(axis, val);
}

AxisRelEndEvent::AxisRelEndEvent(EAxisRel axis) : axis(axis) {}

bool AxisRelEndEvent::apply(EventListenerBase &listener) {
    return listener.on_rel_axis_end(axis);
}
