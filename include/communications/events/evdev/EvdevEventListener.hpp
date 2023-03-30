#pragma once

#include "../EventListenerBase.hpp"
#include "../AxisInputProcessor.hpp"

#include <optional>

class EvdevEventListener : public EventListenerBase {
    struct libevdev *dev;
    struct libevdev_uinput *uidev;

    AxisInputProcessor ax_x, ax_y, ax_wheel, ax_hwheel;

    std::optional<int> button_to_code(EButton btn);
    std::optional<std::tuple<AxisInputProcessor&, int>> get_axis_elements(EAxisRel axis);
public:
    EvdevEventListener();
    ~EvdevEventListener();

    bool on_button_event(EButton btn, bool is_pressed) override;
    bool on_rel_axis(EAxisRel axis, float val) override;
    bool on_rel_axis_end(EAxisRel axis) override;

    void flush() override;
};
