#pragma once
#include "../IWebSocketClient.hpp"

#include <utils/AtomicWrapper.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

enum states {
    CONNECTING,
    OPENED,
    FAILED,
    CLOSED
};

class WebsocketppClient : public IWebSocketClient {

    using client_t = websocketpp::client<websocketpp::config::asio_client>;
    client_t client;
    websocketpp::connection_hdl handle;
    AtomicWrapper<int> state{CONNECTING};
    std::thread t;
public:
    WebsocketppClient(const std::string& uri, message_callback_t msg_callback);
    ~WebsocketppClient();
    bool send(const std::string& msg);
};
