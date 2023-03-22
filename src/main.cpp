#include <cmath>
#include <iostream>
#include <thread>
#include <array>
#include <future>
#include <sstream>

#include <websocket/websocketpp/server/GenWSPPServer.hpp>

void exec(const std::string& cmd) {
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
}

using ss = std::stringstream;

double x{0}, y{0};

void mouse_move_end_apply() {
    x = y = 0;
}

void mouse_move_apply(ss& s) {
    double x_in, y_in;
    s >> x_in >> y_in;

    x += x_in;
    y += y_in;

    double x_out = std::round(x / 200);
    double y_out = std::round(y / 200);

    x -= x_out * 200;
    y -= y_out * 200;

    int x_out_int = x_out;
    int y_out_int = y_out;
    exec((ss() <<
          "xdotool mousemove_relative -- "
          << x_out_int << " "
          << y_out_int).str());
}

std::unordered_map<std::string, int> btn_map{
    {"lmb", 1},
    {"lmb", 2},
    {"rmb", 3}
};

void press_apply(ss& s) {
    std::string btn;
    s >> btn;

    if(!btn_map.contains(btn)) {
        std::cout << "button " << btn << " not found" << std::endl;
        return;
    }

    auto cmd = (ss() << "xdotool mousedown " << btn_map[btn]).str();
    std::cout << cmd << std::endl;
    exec(cmd);
}
void release_apply(ss& s) {
    std::string btn;
    s >> btn;

    if(!btn_map.contains(btn)) {
        std::cout << "button " << btn << " not found" << std::endl;
        return;
    }

    auto cmd = (ss() << "xdotool mouseup " << btn_map[btn]).str();
    std::cout << cmd << std::endl;
    exec(cmd);
}

void interpret_message(const std::string& msg) {
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
