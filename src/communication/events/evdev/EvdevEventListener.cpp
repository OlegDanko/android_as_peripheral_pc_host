#include <communications/events/evdev/EvdevEventListener.hpp>

#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>
#include <iostream>
#include <tuple>

EvdevEventListener::EvdevEventListener() {
    int err;

    dev = libevdev_new();
    libevdev_set_name(dev, "test device");
    libevdev_enable_event_type(dev, EV_REL);
    libevdev_enable_event_code(dev, EV_REL, REL_X, NULL);
    libevdev_enable_event_code(dev, EV_REL, REL_Y, NULL);
    libevdev_enable_event_code(dev, EV_REL, REL_WHEEL_HI_RES, NULL);
    libevdev_enable_event_type(dev, EV_KEY);
    libevdev_enable_event_code(dev, EV_KEY, BTN_LEFT, NULL);
    libevdev_enable_event_code(dev, EV_KEY, BTN_MIDDLE, NULL);
    libevdev_enable_event_code(dev, EV_KEY, BTN_RIGHT, NULL);

    err = libevdev_uinput_create_from_device(dev,
                                             LIBEVDEV_UINPUT_OPEN_MANAGED,
                                             &uidev);
    if (err != 0) {
        std::cout << "failed to open device" << std::endl;
    }
}

EvdevEventListener::~EvdevEventListener() {
    libevdev_uinput_destroy(uidev);
}

std::optional<int> EvdevEventListener::button_to_code(EButton btn) {
    switch(btn) {
    case lmb: return BTN_LEFT;
    case mmb: return BTN_MIDDLE;
    case rmb: return BTN_RIGHT;
    default:
        return {};
    }
}

bool EvdevEventListener::on_button_event(EButton btn, bool is_pressed) {
    int val = is_pressed ? 1 : 0;
    if(auto code_opt = button_to_code(btn); code_opt) {
        libevdev_uinput_write_event(uidev, EV_KEY, code_opt.value(), val);
        return true;
    }
    return false;
}

std::optional<std::tuple<AxisInputProcessor &, int> >
EvdevEventListener::get_axis_elements(EAxisRel axis) {
    switch(axis) {
    case mouse_x_axis:
        return {{ax_x, REL_X}};
    case mouse_y_axis:
        return {{ax_y, REL_Y}};
    default:
        return {};
    }
}

bool EvdevEventListener::on_rel_axis(EAxisRel axis, float val) {
    if(auto ax_elems_opt = get_axis_elements(axis); ax_elems_opt) {
        auto [ax_proc, code] = ax_elems_opt.value();
        libevdev_uinput_write_event(uidev, EV_REL, code, ax_proc.calc(val));
        return true;
    }
    return false;

}

bool EvdevEventListener::on_rel_axis_end(EAxisRel axis) {
    if(auto ax_elems_opt = get_axis_elements(axis); ax_elems_opt) {
        auto [ax_proc, _] = ax_elems_opt.value();
        ax_proc.clear();
        return true;
    }
    return false;
}

void EvdevEventListener::flush() {
    libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
}
