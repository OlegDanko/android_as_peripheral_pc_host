#include <websocket/websocketpp/WebSocketppClient.hpp>

std::unique_ptr<IWebSocketClient> gen_websocket_client(const std::string& uri,
                                                       message_callback_t callback) {
    return std::make_unique<WebsocketppClient>(uri, callback);
}
