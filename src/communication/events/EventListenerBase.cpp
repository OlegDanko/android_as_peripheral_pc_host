#include <communications/events/EventListenerBase.hpp>

bool EventListenerBase::on_button_event(EButton button, bool is_pressed)
{ return false; }

bool EventListenerBase::on_rel_axis(EAxisRel axis, float val)
{ return false; }

bool EventListenerBase::on_rel_axis_end(EAxisRel axis)
{ return false; }
