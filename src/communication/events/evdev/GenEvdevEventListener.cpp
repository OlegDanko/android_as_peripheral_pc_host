#include <communications/events/evdev/GenEvdevEventListener.hpp>
#include <communications/events/evdev/EvdevEventListener.hpp>

std::unique_ptr<EventListenerBase> gen_evdev_event_listener() {
    return std::make_unique<EvdevEventListener>();
}
