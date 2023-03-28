#include <cmath>
#include <linux/uinput.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdlib>
#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>
#include <thread>
#include <iostream>
#include <sstream>
#include <vector>
#include <websocket/websocketpp/server/GenWSPPServer.hpp>

class Input {
    struct libevdev *dev;
    struct libevdev_uinput *uidev;
public:
    Input() {
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
//            return err;
    }
    ~Input() {
        libevdev_uinput_destroy(uidev);
    }

    void move_mouse(int x, int y) {
        libevdev_uinput_write_event(uidev, EV_REL, REL_X, x);
        libevdev_uinput_write_event(uidev, EV_REL, REL_Y, y);
        libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
    }

    void button(int btn, bool pressed) {
        int val = pressed ? 1 : 0;
        switch(btn) {
        case 1:
            libevdev_uinput_write_event(uidev, EV_KEY, BTN_LEFT, val);
            break;
        case 2:
            libevdev_uinput_write_event(uidev, EV_KEY, BTN_MIDDLE, val);
            break;
        case 3:
            libevdev_uinput_write_event(uidev, EV_KEY, BTN_RIGHT, val);
            break;
        }
        libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
    }

};

Input input_clinent;

double x{0}, y{0};

void mouse_move_end_apply() {
    x = y = 0;
}

void mouse_move_apply(std::stringstream& s) {
    double x_in = 0.0, y_in = 0.0;
    s >> x_in >> y_in;

    std::cout << x_in << " " << y_in << std::endl;

    double exp_base = 1.00002;
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

    input_clinent.move_mouse(x_out_int, y_out_int);
}

std::unordered_map<std::string, int> btn_map{
    {"lmb", 1},
    {"mmb", 2},
    {"rmb", 3}
};

void press_apply(std::stringstream& s) {
    std::string btn;
    s >> btn;

    if(!btn_map.contains(btn)) {
        std::cout << "button " << btn << " not found" << std::endl;
        return;
    }

    input_clinent.button(btn_map[btn], 1);
}
void release_apply(std::stringstream& s) {
    std::string btn;
    s >> btn;

    if(!btn_map.contains(btn)) {
        std::cout << "button " << btn << " not found" << std::endl;
        return;
    }

    input_clinent.button(btn_map[btn], 0);
}

void interpret_message(const std::string& msg) {
    std::stringstream s(msg);
    std::string cmd;
    while(!(s >> cmd).fail()) {
        if (0 == cmd.compare("mouse_mv")) {
            mouse_move_apply(s);
            continue;
        }
        if (0 == cmd.compare("mouse_end")) {
            mouse_move_end_apply();
            continue;
        }
        if (0 == cmd.compare("press")) {
            press_apply(s);
            continue;
        }
        if (0 == cmd.compare("release")) {
            release_apply(s);
            continue;
        }
        std::cout << "Couldn't understand: " << msg << std::endl;
        return;
    }
}

int main(int argc, char* argv[]) {
    std::vector<std::unique_ptr<IConnectionProvider>> connections;

    auto ws = gen_websocketpp_server(33333, [&](std::unique_ptr<IConnectionProvider> c){
        c->init_message_callback([](const auto& msg) {
            static auto last = std::chrono::steady_clock::now();
            auto current = std::chrono::steady_clock::now();
            std::cout
                    << std::chrono::duration<double, std::milli>(current - last).count()
                    << std::endl;
            last = current;
            interpret_message(msg);
        });
        connections.push_back(std::move(c));
    });


    while(true) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
//        std::this_thread::sleep_for(std::chrono::milliseconds(1));
//        auto t = std::chrono::steady_clock::now().time_since_epoch().count() / 100000.0;
//        input_clinent.move_mouse(std::sin(t)*10, std::cos(t)*10 );
    }

    return 0;
}

