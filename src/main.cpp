#include <iostream>
#include <thread>
#include <array>
#include <future>

#include <websocket/websocketpp/server/GenWSPPServer.hpp>

template<size_t S>
struct s {
  static void print() { std::cout << S << std::endl; }
};

int main(int argc, char* argv[]) {
    std::vector<std::unique_ptr<IConnectionProvider>> connections;

    if(true) {
    auto ws = gen_websocketpp_server(33333, [&](std::unique_ptr<IConnectionProvider> c){
        connections.push_back(std::move(c));
    });

    }

    return 0;
}
