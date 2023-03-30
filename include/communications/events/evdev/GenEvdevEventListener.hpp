#pragma once

#include "../EventListenerBase.hpp"
#include <memory>

std::unique_ptr<EventListenerBase> gen_evdev_event_listener();
