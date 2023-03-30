#pragma once

#include "IEvent.hpp"
#include <queue>
#include <memory>

using event_queue_t = std::queue<std::unique_ptr<IEvent>>;

event_queue_t read_events(const std::string& msg);
