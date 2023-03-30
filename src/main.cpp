#include <websocket/websocketpp/server/GenWSPPServer.hpp>
#include <communications/events/evdev/GenEvdevEventListener.hpp>
#include <communications/events/EventReader.hpp>

#include <thread>
#include <chrono>
#include <iostream>

int main(int argc, char* argv[]) {
    std::vector<std::unique_ptr<IConnectionProvider>> connections;
    auto event_listiner = gen_evdev_event_listener();

    auto ws = gen_websocketpp_server(33333, [&](std::unique_ptr<IConnectionProvider> c){
        c->init_message_callback([&event_listiner](const auto& msg) {
            auto start = std::chrono::steady_clock::now();
            auto queue = read_events(msg);
            if(queue.empty()) return;

            do {
                queue.front()->apply(*event_listiner);
                queue.pop();
            } while(!queue.empty());
            event_listiner->flush();
            auto end = std::chrono::steady_clock::now();
            auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            if(elapsed_ms > 0)
                std::cout << elapsed_ms << std::endl;
        });
        connections.push_back(std::move(c));
    });

    while(true) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}
