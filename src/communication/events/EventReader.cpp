#include <communications/events/EventReader.hpp>
#include <communications/events/Event.hpp>
#include <sstream>
#include <optional>
#include <functional>
#include <unordered_map>
#include <iostream>

bool read_mouse_move_event(std::stringstream& str, event_queue_t& queue) {
    float x_in = 0.0, y_in = 0.0;
    if((str >> x_in >> y_in).fail())
        return false;
    queue.push(std::make_unique<AxisRelEvent>(mouse_x_axis, x_in));
    queue.push(std::make_unique<AxisRelEvent>(mouse_y_axis, y_in));
    return true;
}

bool read_mouse_move_end_event(std::stringstream& str, event_queue_t& queue) {
    queue.push(std::make_unique<AxisRelEndEvent>(mouse_x_axis));
    queue.push(std::make_unique<AxisRelEndEvent>(mouse_y_axis));
    return true;
}

std::unordered_map<std::string, EButton> btn_map{
    {"lmb", lmb},
    {"mmb", mmb},
    {"rmb", rmb}
};

std::optional<EButton> string_to_button(const std::string& btn) {
    if(!btn_map.contains(btn)) {
        return {};
    }
    return { btn_map[btn] };
}


bool read_button_event(std::stringstream& str, event_queue_t& queue, bool is_pressed) {
    std::string btn_str;

    if((str>>btn_str).fail())
        return false;

    if(auto btn_opt = string_to_button(btn_str); btn_opt) {
        queue.push(std::make_unique<ButtonEvent>(btn_opt.value(), is_pressed));
        return true;
    }

    return false;
}

bool read_button_press_event(std::stringstream& str, event_queue_t& queue) {
    return read_button_event(str, queue, true);
}

bool read_button_release_event(std::stringstream& str, event_queue_t& queue) {
    return read_button_event(str, queue, false);
}


event_queue_t read_events(const std::string &msg) {
    std::stringstream s(msg);
    std::string cmd;

    using event_read_fn_t = std::function<bool(std::stringstream&, event_queue_t&)>;

    std::unordered_map<std::string, event_read_fn_t> read_fn_map {
        {"mouse_mv", read_mouse_move_event},
        {"mouse_end", read_mouse_move_end_event},
        {"press", read_button_press_event},
        {"release", read_button_release_event},
    };

    event_queue_t queue;

    while(!(s >> cmd).fail()) {
        if(!read_fn_map.contains(cmd)) {
            std::cout << "Unexpected command: " << msg << std::endl;
            return queue;
        }
        read_fn_map[cmd](s, queue);
    }
    return queue;
}
