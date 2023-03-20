//#include <websocket/IWebSocketClient.hpp>
//#include <websocket/websocketpp/WebSocketppServer.hpp>
#include <iostream>
#include <thread>
#include <array>
#include <future>

#include <websocket/websocketpp/server/GenWSPPServer.hpp>


int main(int argc, char* argv[]) {
    std::vector<std::unique_ptr<IConnectionProvider>> connections;

    if(true) {
    auto ws = gen_websocketpp_server(33333, [&](std::unique_ptr<IConnectionProvider> c){
        connections.push_back(std::move(c));
    });

    std::this_thread::sleep_for(std::chrono::seconds(10));
    }
    std::cout << "The end" << std::endl;

//    using WSServer_t = WebSocketppServer<server_t, conn_hdl_t>;
//    using WSServer_t = WebSocketppServer<WSServerMock, WSConnectionHandleMock, 200>;

//    WSServer_t server(2000, [](std::unique_ptr<IConnectionProvider> conn_provider){
//    });
//    WSConnectionHandleMock x{10};


    return 0;
}
