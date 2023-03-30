#pragma once

enum EButton {
    lmb = 1,
    mmb = 2,
    rmb = 3,
};

enum EAxisRel {
    mouse_x_axis = 1,
    mouse_y_axis,
    scroll_v_axis,
    scroll_h_axis,
};

struct EventListenerBase {
    virtual bool on_button_event(EButton button, bool is_pressed);
    virtual bool on_rel_axis(EAxisRel axis, float val);
    virtual bool on_rel_axis_end(EAxisRel axis);
    virtual void flush() = 0;
    virtual ~EventListenerBase() = default;
};
