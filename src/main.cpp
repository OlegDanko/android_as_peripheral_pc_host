#include <cmath>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdlib>

#include <thread>
#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <optional>
#include <websocket/websocketpp/server/GenWSPPServer.hpp>
#include <communications/events/evdev/GenEvdevEventListener.hpp>
#include <communications/events/EventReader.hpp>

auto event_listiner = gen_evdev_event_listener();

void process_event_message(const std::string& msg) {
    auto queue = read_events(msg);

    if(queue.empty()) return;

    do {
        queue.front()->apply(*event_listiner);
        queue.pop();
    } while(!queue.empty());
    event_listiner->flush();
}

int main(int argc, char* argv[]) {
    std::vector<std::unique_ptr<IConnectionProvider>> connections;

    auto ws = gen_websocketpp_server(33333, [&](std::unique_ptr<IConnectionProvider> c){
        c->init_message_callback([](const auto& msg) {
            process_events(msg);
        });
        connections.push_back(std::move(c));
    });

    while(true) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}
