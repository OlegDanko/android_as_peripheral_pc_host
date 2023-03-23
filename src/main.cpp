#include <cmath>
#include <iostream>
#include <thread>
#include <array>
#include <future>
#include <sstream>
#include <optional>
#include <iomanip>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>

#include <websocket/websocketpp/server/GenWSPPServer.hpp>

class InputClientX11 {
    Display *display;
    Window root_window;
public:
    InputClientX11()
        : display(XOpenDisplay(NULL))
        , root_window(DefaultRootWindow(display)) {}
    ~InputClientX11() {
        XCloseDisplay(display);
    }

    std::optional<XEvent> query_pointer(Window& window) {
        XEvent event;
        if(0 == XQueryPointer(display,
                              window,
                              &event.xbutton.root,
                              &event.xbutton.subwindow,
                              &event.xbutton.x_root,
                              &event.xbutton.y_root,
                              &event.xbutton.x,
                              &event.xbutton.y,
                              &event.xbutton.state)) {
            return {};
        }
        return {event};
    }

    std::optional<std::tuple<int, int>> get_pointer_position() {
        if(auto event_opt = query_pointer(root_window); event_opt) {
            auto event_btn = event_opt.value().xbutton;
            return {std::make_tuple(event_btn.x_root, event_btn.y_root)};
        }

        std::cout << "failed onto retreive current pointer position" << std::endl;
        return {};

    }

    void set_pointer_position(Display *display, Window& root, int x, int y) {
        XWarpPointer(display, None, root, 0, 0, 0, 0, x, y);
        XFlush(display);
    }

    void move_cursor(int x, int y) {
        XTestFakeRelativeMotionEvent(display, x, y, 0);
        XFlush(display);
        return;
        if(auto p_opt = get_pointer_position(); p_opt) {
            auto [pos_x, pos_y] = p_opt.value();
            set_pointer_position(display, root_window, pos_x + x, pos_y + y);
        }
    }

    std::optional<XEvent> query_pointer_leaf_window() {
        XEvent event;
        event.xbutton.subwindow = root_window;
        while (event.xbutton.subwindow) {
            std::cout << "checking " << event.xbutton.subwindow << std::endl;
            auto event_opt = query_pointer(event.xbutton.subwindow);
            if(!event_opt) {
                std::cout << "failed to query leaf event" << std::endl;
                return {};
            }
            event = event_opt.value();
        }
        std::cout << "Window containing pointer is " << event.xbutton.window << std::endl;
        return {event};
    }

    void button_press(int button) {
        XTestFakeButtonEvent(display, button, True, CurrentTime);
        XFlush(display);
    }
    void button_release(int button) {
        XTestFakeButtonEvent(display, button, False, CurrentTime);
        XFlush(display);
    }
};

InputClientX11 input_clint;

double x{0}, y{0};

void mouse_move_end_apply() {
    x = y = 0;
}

using ss = std::stringstream;

void mouse_move_apply(ss& s) {
    double x_in, y_in;
    s >> x_in >> y_in;

    double exp_base = 1.00005;
    x_in *= std::pow(exp_base, std::abs(x_in));
    y_in *= std::pow(exp_base, std::abs(y_in));

    x += x_in;
    y += y_in;

    int div = 200;

    double x_out = std::round(x / div);
    double y_out = std::round(y / div);

    x -= x_out * div;
    y -= y_out * div;

    int x_out_int = x_out;
    int y_out_int = y_out;

    input_clint.move_cursor(x_out_int, y_out_int);
}


std::unordered_map<std::string, int> btn_map{
    {"lmb", 1},
    {"mmb", 2},
    {"rmb", 3}
};

void press_apply(ss& s) {
    std::string btn;
    s >> btn;

    if(!btn_map.contains(btn)) {
        std::cout << "button " << btn << " not found" << std::endl;
        return;
    }

    input_clint.button_press(btn_map[btn]);
}
void release_apply(ss& s) {
    std::string btn;
    s >> btn;

    if(!btn_map.contains(btn)) {
        std::cout << "button " << btn << " not found" << std::endl;
        return;
    }

    input_clint.button_release(btn_map[btn]);
}

void interpret_message(const std::string& msg) {
    static const auto start = std::chrono::steady_clock::now();

    auto since_start = std::chrono::steady_clock::now() - start;

    std::cout << since_start.count()
              << std::endl;
    ss s(msg);
    std::string cmd;
    s >> cmd;
    if (0 == cmd.compare("mouse_mv")) {
        mouse_move_apply(s);
        return;
    }
    if (0 == cmd.compare("mouse_end")) {
        mouse_move_end_apply();
        return;
    }
    if (0 == cmd.compare("press")) {
        press_apply(s);
        return;
    }
    if (0 == cmd.compare("release")) {
        release_apply(s);
        return;
    }
    std::cout << "Message received: " << msg << std::endl;
}

int main(int argc, char* argv[]) {
    std::vector<std::unique_ptr<IConnectionProvider>> connections;

    auto ws = gen_websocketpp_server(33333, [&](std::unique_ptr<IConnectionProvider> c){
        c->init_message_callback([](const auto& msg) {
            interpret_message(msg);
        });
        connections.push_back(std::move(c));
    });


    while(true) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}
