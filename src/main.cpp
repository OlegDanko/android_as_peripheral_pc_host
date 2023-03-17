#include <websocket/IWebSocketClient.hpp>
#include <iostream>
#include <thread>

int main(int argc, char* argv[]) {
    std::string uri = "ws://localhost:33333";

    auto ws_u_ptr = gen_websocket_client(uri, [](const std::string& msg){
        std::cout << "Received: " << msg << std::endl;
    });

    while(ws_u_ptr->send("Hello there!")) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return 0;
}
